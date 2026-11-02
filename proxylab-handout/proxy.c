#include "csapp.h"

/* 留给后续缓存部分；当前已支持每个连接由独立线程处理。 */
#define MAX_CACHE_SIZE 1049000
#define MAX_OBJECT_SIZE 102400
#define MAX_HEADERS 65536

static const char *user_agent_hdr = "User-Agent: Mozilla/5.0 (X11; Linux x86_64; rv:10.0.3) Gecko/20120305 Firefox/10.0.3\r\n";

/* 向当前客户端返回一个无正文的错误响应，不退出整个代理。 */
static void clienterror(int fd, const char *status)
{
    char buf[MAXLINE];
    int n = snprintf(buf, sizeof(buf),
                     "HTTP/1.0 %s\r\nContent-Length: 0\r\n"
                     "Connection: close\r\n\r\n", status);
    rio_writen(fd, buf, n);
}

/* 将 URL 拆成目标主机、端口、路径，authority 用于补写 Host。
 * 例：http://localhost:15213/home.html
 * hostname="localhost", port="15213", path="/home.html",
 * authority="localhost:15213"。所有输出数组的容量均为 MAXLINE。
 */
static int parse_uri(const char *uri, char *hostname, char *port,
                     char *path, char *authority)
{
    const char *start, *rest;
    size_t len;

    if (strncasecmp(uri, "http://", 7) != 0)
        return -1;
    start = uri + 7;
    len = strcspn(start, "/?#");
    if (len == 0 || len >= MAXLINE)
        return -1;

    memcpy(authority, start, len);
    authority[len] = '\0';
    strcpy(hostname, authority);
    strcpy(port, "80");  /* URL 未指定端口时使用 HTTP 默认端口。 */
    if (strchr(hostname, '@'))
        return -1;

    if (hostname[0] == '[') {
        /* IPv6 URL 形如 [::1]:15213，连接时主机名需要去掉方括号。 */
        char *end = strchr(hostname, ']');
        if (!end || (end[1] != '\0' && end[1] != ':'))
            return -1;
        if (end[1] == ':')
            strcpy(port, end + 2);
        *end = '\0';
        memmove(hostname, hostname + 1, strlen(hostname));
    } else {
        char *colon = strchr(hostname, ':');
        if (colon) {
            *colon = '\0';
            strcpy(port, colon + 1);
        }
    }

    if (!hostname[0] || !port[0])
        return -1;
    for (size_t i = 0; port[i]; i++) {
        if (!isdigit((unsigned char)port[i]))
            return -1;
    }
    /* 先限制位数，避免超长数字造成整数转换溢出。 */
    if (strlen(port) > 5 || atoi(port) < 1 || atoi(port) > 65535)
        return -1;

    rest = start + len;
    len = strcspn(rest, "#");  /* 片段标识不发送给目标服务器。 */
    if (*rest == '/') {
        memcpy(path, rest, len);
        path[len] = '\0';
    } else {
        /* 没有路径时使用 /；只有查询参数 ?a=1 时使用 /?a=1。 */
        if (len + 1 >= MAXLINE)
            return -1;
        path[0] = '/';
        memcpy(path + 1, rest, len);
        path[len + 1] = '\0';
    }
    return 0;
}

/* 读完请求头，保留需要原样转发的字段。
 * User-Agent、Connection、Proxy-Connection 由代理统一填写。
 * 保留客户端提供的 Host；客户端没有提供时，稍后根据 URL 补上。
 */
static int read_requesthdrs(rio_t *rio, char *headers, int *has_host)
{
    char buf[MAXLINE];
    size_t used = 0, total = 0;
    ssize_t n;

    headers[0] = '\0';
    *has_host = 0;
    while ((n = rio_readlineb(rio, buf, sizeof(buf))) > 0) {
        char *colon;
        size_t name_len;
        total += n;
        if (total >= MAX_HEADERS || n < 2 ||
            buf[n - 2] != '\r' || buf[n - 1] != '\n')
            return -1;
        if (!strcmp(buf, "\r\n"))
            return 0;  /* 空行表示请求头结束。 */

        colon = strchr(buf, ':');
        if (!colon || colon == buf)
            return -1;
        name_len = colon - buf;
        if ((name_len == 10 && !strncasecmp(buf, "User-Agent", 10)) ||
            (name_len == 10 && !strncasecmp(buf, "Connection", 10)) ||
            (name_len == 16 && !strncasecmp(buf, "Proxy-Connection", 16)))
            continue;
        if (name_len == 4 && !strncasecmp(buf, "Host", 4))
            *has_host = 1;

        memcpy(headers + used, buf, n);
        used += n;
        headers[used] = '\0';
    }
    return -1;  /* 读取出错，或者对端尚未发送完整请求头就断开。 */
}

/* 按你原来的流程处理一次请求。
 * clientfd 由工作线程关闭，目标服务器的 conn_ps_fd 由本函数关闭。
 * 缓冲区和 RIO 状态都是局部变量，各个线程之间不会共用这些请求数据。
 */
static void handle_client(int clientfd)
{
    char buf[MAXLINE], method[MAXLINE], uri[MAXLINE], version[MAXLINE];
    char extra[2];
    char server_name[MAXLINE], server_port[MAXLINE], path[MAXLINE];
    char authority[MAXLINE], headers[MAX_HEADERS], request[MAXLINE + 64];
    rio_t client_rio, server_rio;
    ssize_t n;
    int conn_ps_fd, has_host;

    /* 1. 解析：读取请求行，拆出方法、完整 URL 和版本。
     * 小写 RIO 将错误返回给调用者，不会因某个连接出错而退出整个程序。
     */
    rio_readinitb(&client_rio, clientfd);
    n = rio_readlineb(&client_rio, buf, sizeof(buf));
    if (n <= 0)
        return;
    /* 8191 = MAXLINE - 1，为字符串结尾保留空间；extra 用于发现多余字段。 */
    if (n < 2 || buf[n - 2] != '\r' || buf[n - 1] != '\n' ||
        sscanf(buf, "%8191s %8191s %8191s %1s",
               method, uri, version, extra) != 3 ||
        (strcmp(version, "HTTP/1.0") && strcmp(version, "HTTP/1.1"))) {
        clienterror(clientfd, "400 Bad Request");
        return;
    }
    if (strcasecmp(method, "GET")) {
        clienterror(clientfd, "501 Not Implemented");
        return;
    }
    if (parse_uri(uri, server_name, server_port, path, authority) < 0 ||
        read_requesthdrs(&client_rio, headers, &has_host) < 0) {
        clienterror(clientfd, "400 Bad Request");
        return;
    }

    /* 2. 连接 server：使用从 URL 提取出的主机名和端口。 */
    conn_ps_fd = open_clientfd(server_name, server_port);
    if (conn_ps_fd < 0) {
        clienterror(clientfd, "502 Bad Gateway");
        return;
    }

    /* 3. 发送请求：完整 URL 改成 path，协议版本统一为 HTTP/1.0。
     * snprintf 在内存里生成字符串，rio_writen 才真正向连接写入数据。
     */
    snprintf(request, sizeof(request), "GET %s HTTP/1.0\r\n", path);
    if (rio_writen(conn_ps_fd, request, strlen(request)) < 0)
        goto done;
    if (!has_host) {
        snprintf(request, sizeof(request), "Host: %s\r\n", authority);
        if (rio_writen(conn_ps_fd, request, strlen(request)) < 0)
            goto done;
    }
    if (rio_writen(conn_ps_fd, (void *)user_agent_hdr, strlen(user_agent_hdr)) < 0 ||
        rio_writen(conn_ps_fd, "Connection: close\r\n", 19) < 0 ||
        rio_writen(conn_ps_fd, "Proxy-Connection: close\r\n", 25) < 0 ||
        rio_writen(conn_ps_fd, headers, strlen(headers)) < 0 ||
        rio_writen(conn_ps_fd, "\r\n", 2) < 0)
        goto done;

    /* 4. 接收响应 + 5. 发送响应：读一块，转发一块，不保存整个响应。
     * 状态行、响应头和正文全部原样转发；正文可能是图片等二进制内容，
     * 因此使用实际读取数量 n，不能用 strlen(buf)。
     * HTTP/1.0 配合 Connection: close，通过服务器关闭连接得到 EOF。
     */
    rio_readinitb(&server_rio, conn_ps_fd);
    while ((n = rio_readnb(&server_rio, buf, sizeof(buf))) > 0) {
        if (rio_writen(clientfd, buf, n) < 0)
            break;
    }

done:
    /* 正常结束或写入出错都关闭服务器连接，避免泄漏描述符。
     * goto 在这里仅用于共享清理代码，不会结束整个代理。
     */
    close(conn_ps_fd);
}

/* pthread_create 要求线程入口的类型为 void *函数名(void *)。
 * 每个连接都有单独分配的参数，避免主线程的下一次 accept 覆盖旧连接描述符。
 */
static void *thread(void *arg)
{
    int clientfd = *(int *)arg;
    free(arg);  /* 参数已复制到局部变量，立即释放传参用的内存。 */

    handle_client(clientfd);
    close(clientfd);
    return NULL;  /* 创建时已指定分离状态，退出后自动回收线程资源。 */
}

int main(int argc, char **argv)
{
    /* 主线程只接受连接、创建工作线程；请求转发由各工作线程并行执行。 */
    struct sockaddr_storage clientaddr;
    socklen_t clientlen;
    int listen_p_fd, conn_cp_fd;
    int rc;
    pthread_attr_t attr;
    char *proxy_port;

    if (argc != 2) {
        fprintf(stderr, "usage: %s <port>\n", argv[0]);
        return 1;
    }
    proxy_port = argv[1];

    /* 对端提前断开时，写操作可能触发 SIGPIPE；忽略信号后检查写入返回值。 */
    signal(SIGPIPE, SIG_IGN);

    /* 监听 client：只需要自己的端口，不必预先知道客户端名字。 */
    listen_p_fd = open_listenfd(proxy_port);
    if (listen_p_fd < 0) {
        fprintf(stderr, "Cannot listen on port %s\n", proxy_port);
        return 1;
    }

    /* 在线程创建时就设为 detached，无须 pthread_join 或在线程中再次 detach。
     * pthread_* 出错时直接返回错误码，应使用 strerror(rc)，而不是 perror。
     */
    rc = pthread_attr_init(&attr);
    if (rc != 0) {
        fprintf(stderr, "pthread_attr_init: %s\n", strerror(rc));
        close(listen_p_fd);
        return 1;
    }
    rc = pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
    if (rc != 0) {
        fprintf(stderr, "pthread_attr_setdetachstate: %s\n", strerror(rc));
        pthread_attr_destroy(&attr);
        close(listen_p_fd);
        return 1;
    }

    while (1) {
        int *clientfd_ptr;
        pthread_t tid;

        clientlen = sizeof(clientaddr);
        /* 接受客户端主动发起的连接，取得与它通信的描述符。 */
        conn_cp_fd = accept(listen_p_fd, (SA *)&clientaddr, &clientlen);
        if (conn_cp_fd < 0) {
            perror("accept");
            continue;
        }
        /* 不能传 &conn_cp_fd：主线程会在下一轮循环改写它。
         * 每个线程获得独立的一份参数，并负责释放它及关闭客户端连接。
         */
        clientfd_ptr = malloc(sizeof(*clientfd_ptr));
        if (clientfd_ptr == NULL) {
            close(conn_cp_fd);
            continue;
        }
        *clientfd_ptr = conn_cp_fd;
        rc = pthread_create(&tid, &attr, thread, clientfd_ptr);
        if (rc != 0) {
            /* 创建失败，没有工作线程接手，因此由主线程完成清理。 */
            fprintf(stderr, "pthread_create: %s\n", strerror(rc));
            free(clientfd_ptr);
            close(conn_cp_fd);
        }
        /* 创建成功后继续接受新连接；主线程不能关闭工作线程正在使用的描述符。 */
    }
}
