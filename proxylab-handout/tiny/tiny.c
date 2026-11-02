/* $begin tinymain */
/*
 * tiny.c - A simple, iterative HTTP/1.0 Web server that uses the 
 *     GET method to serve static and dynamic content.
 *
 * Updated 11/2019 droh 
 *   - Fixed sprintf() aliasing issue in serve_static(), and clienterror().
 * 中文：tiny.c：简单的迭代式 HTTP/1.0 Web 服务器，用 GET 提供静态与动态内容。2019 年
 * 11 月由 droh 更新：修复 serve_static() 与 clienterror() 中 sprintf() 的别名问题。
 *
 * 阅读路线：
 *   main：监听端口 -> 接受连接 -> 调用 doit -> 关闭这次连接。
 *   doit：读取请求 -> 解析 URI -> 检查本地文件 -> 发送静态文件或运行 CGI。
 *
 * “迭代式”表示一次处理一个连接：doit 返回以后，才会接受下一个连接。
 * 静态内容是磁盘上已有的文件；动态内容是 CGI 程序运行后生成的输出。
 * Tiny 是目标 Web 服务器，因此 URI 会映射到本地文件；Proxy Lab 中的代理则需要
 * 从 URL 提取目标主机和端口，连接目标服务器，再转发请求和响应。
 *
 * 本文件使用 Open、Accept、Rio_* 等大写包装函数。它们在错误时可能退出整个进程，
 * 学习代理实现时要注意这一点：普通连接的错误应尽量只结束当前连接。
 */
#include "csapp.h"

void doit(int fd);
void read_requesthdrs(rio_t *rp);
int parse_uri(char *uri, char *filename, char *cgiargs);
void serve_static(int fd, char *filename, int filesize);
void get_filetype(char *filename, char *filetype);
void serve_dynamic(int fd, char *filename, char *cgiargs);
void clienterror(int fd, char *cause, char *errnum, 
		 char *shortmsg, char *longmsg);

int main(int argc, char **argv) 
{
    /* listenfd 用于等待新连接；connfd 用于和某个已连接的客户端收发数据。 */
    int listenfd, connfd;
    /* 保存客户端的主机名和端口，供日志输出使用。 */
    char hostname[MAXLINE], port[MAXLINE];
    socklen_t clientlen;
    /* 通用地址存储空间，可以容纳 IPv4 或 IPv6 客户端地址。 */
    struct sockaddr_storage clientaddr;

    /* Check command line args
     * 运行方式例如 ./tiny 15213：argv[0] 是程序名，argv[1] 是监听端口字符串。
     */
    if (argc != 2) {
	fprintf(stderr, "usage: %s <port>\n", argv[0]);
	exit(1);
    }

    /* 内部完成地址查询、socket、bind 和 listen，返回监听描述符。
     * 此时只是准备好接受连接，还没有取得与具体客户端通信的描述符。
     */
    listenfd = Open_listenfd(argv[1]);
    while (1) {
	/* Accept 会将客户端地址写入 clientaddr，并更新 clientlen。
	 * 因为 clientlen 是输入/输出参数，每次调用前都要重新设置可用空间大小。
	 */
	clientlen = sizeof(clientaddr);
	/* 没有待接受的连接时会阻塞等待；成功后返回一个新的 connfd。
	 * SA 是 struct sockaddr 的别名，这里通过类型转换传给通用套接字接口。
	 */
	connfd = Accept(listenfd, (SA *)&clientaddr, &clientlen); //line:netp:tiny:accept
        /* 将客户端地址转成便于打印的主机名和端口。
         * 此处 port 是客户端这一端的端口，不是 Tiny 自己的监听端口。
         */
        Getnameinfo((SA *) &clientaddr, clientlen, hostname, MAXLINE, 
                    port, MAXLINE, 0);
        printf("Accepted connection from (%s, %s)\n", hostname, port);
	/* 当前线程直接处理请求，因此请求没有处理完时不会进入下一次 Accept。 */
	doit(connfd);                                             //line:netp:tiny:doit
	/* 一次请求/响应结束后关闭当前连接；listenfd 保留，继续接受新连接。 */
	Close(connfd);                                            //line:netp:tiny:close
    }
}
/* $end tinymain */

/*
 * doit - handle one HTTP request/response transaction
 * 中文：doit：处理一次 HTTP 请求/响应事务，fd 是已连接的客户端描述符。
 * 本函数选择并发送响应；连接最终由 main 中的 Close 关闭。
 */
/* $begin doit */
void doit(int fd) 
{
    int is_static;       /* parse_uri 的分类结果：1 为静态，0 为动态。 */
    struct stat sbuf;    /* stat 获取的文件元数据，如类型、权限和大小。 */
    char buf[MAXLINE], method[MAXLINE], uri[MAXLINE], version[MAXLINE];
    char filename[MAXLINE], cgiargs[MAXLINE];
    rio_t rio;          /* 该客户端连接的 RIO 读取状态及内部缓冲区。 */

    /* Read request line and headers
     * 初始化只是将 rio 与 fd 关联，此时不会读取网络数据。
     * 同一条输入流后续都使用这个 rio，才能利用其中已经预读的数据。
     */
    Rio_readinitb(&rio, fd);
    /* 读取第一行。返回 0 表示还没读到数据就到达 EOF，直接结束本次处理。
     * Rio_readlineb 读到 '\n'、缓冲区长度限制或 EOF 时返回，并补字符串结尾 '\0'。
     */
    if (!Rio_readlineb(&rio, buf, MAXLINE))  //line:netp:doit:readrequest
        return;
    printf("%s", buf);
    /* 例如：GET /home.html HTTP/1.1
     * 拆成 method="GET"、uri="/home.html"、version="HTTP/1.1"。
     * sscanf 从已有字符串中解析，不会继续从套接字读取数据。
     * 教学代码没有检查转换项数和字段宽度；不要将其视为完整的健壮解析器。
     */
    sscanf(buf, "%s %s %s", method, uri, version);       //line:netp:doit:parserequest
    /* strcasecmp 忽略大小写比较字符串，相等时返回 0。
     * 因而这里非 0 表示不是 GET，返回 501（未实现该请求方法）。
     */
    if (strcasecmp(method, "GET")) {                     //line:netp:doit:beginrequesterr
        clienterror(fd, method, "501", "Not Implemented",
                    "Tiny does not implement this method");
        return;
    }                                                    //line:netp:doit:endrequesterr
    /* 请求行之后还有请求头，必须继续读到空行。Tiny 只打印这些头，不解释其含义。 */
    read_requesthdrs(&rio);                              //line:netp:doit:readrequesthdrs

    /* Parse URI from GET request
     * 把资源路径转换成相对于服务器当前工作目录的文件名，并提取 CGI 参数。
     */
    is_static = parse_uri(uri, filename, cgiargs);       //line:netp:doit:staticcheck
    /* stat 读取文件元数据，不读取文件正文。失败时本实现统一返回 404。 */
    if (stat(filename, &sbuf) < 0) {                     //line:netp:doit:beginnotfound
	clienterror(fd, filename, "404", "Not found",
		    "Tiny couldn't find this file");
	return;
    }                                                    //line:netp:doit:endnotfound

    if (is_static) { /* Serve static content  中文：提供静态内容。 */          
	/* 静态资源要求是普通文件，且文件所有者的读权限位已设置。
	 * 这里检查的是权限位，并不是完整判断当前进程的实际访问权限。
	 */
	if (!(S_ISREG(sbuf.st_mode)) || !(S_IRUSR & sbuf.st_mode)) { //line:netp:doit:readable
	    clienterror(fd, filename, "403", "Forbidden",
			"Tiny couldn't read the file");
	    return;
	}
	/* st_size 是文件的字节数，用来生成 Content-length 并发送正文。 */
	serve_static(fd, filename, sbuf.st_size);        //line:netp:doit:servestatic
    }
    else { /* Serve dynamic content  中文：提供动态内容。 */
	/* CGI 资源要求是普通文件，且文件所有者的执行权限位已设置。 */
	if (!(S_ISREG(sbuf.st_mode)) || !(S_IXUSR & sbuf.st_mode)) { //line:netp:doit:executable
	    clienterror(fd, filename, "403", "Forbidden",
			"Tiny couldn't run the CGI program");
	    return;
	}
	serve_dynamic(fd, filename, cgiargs);            //line:netp:doit:servedynamic
    }
}
/* $end doit */

/*
 * read_requesthdrs - read HTTP request headers
 * 中文：读取并打印请求头，直到只包含 "\r\n" 的空行。
 * rp 指向 doit 已初始化的 rio，不能重新初始化，否则会丢掉已预读的数据。
 * 注意本教学循环没有检查中途 EOF；客户端未发送完整请求头便断开时，处理不够健壮。
 */
/* $begin read_requesthdrs */
void read_requesthdrs(rio_t *rp) 
{
    char buf[MAXLINE];

    /* doit 已读过请求行，所以这里首先读到的是第一个请求头，或结束请求头的空行。 */
    Rio_readlineb(rp, buf, MAXLINE);
    printf("%s", buf);
    /* strcmp 相等时返回 0：当前行不是空行，就继续读下一行。
     * 每个普通请求头也以 \r\n 结尾，但只有空行的完整内容恰好等于 "\r\n"。
     */
    while(strcmp(buf, "\r\n")) {          //line:netp:readhdrs:checkterm
	Rio_readlineb(rp, buf, MAXLINE);
	printf("%s", buf);
    }
    return;
}
/* $end read_requesthdrs */

/*
 * parse_uri - parse URI into filename and CGI args
 *             return 0 if dynamic content, 1 if static
 * 中文：parse_uri：把 URI 解析为文件名和 CGI 参数；动态内容返回 0，静态内容返回 1。
 * filename 和 cgiargs 是调用者提供的输出缓冲区；动态分支还会原地修改 uri。
 * 这里用 URI 是否包含 "cgi-bin" 简单分类，并非通用的 URL 解析器。
 */
/* $begin parse_uri */
int parse_uri(char *uri, char *filename, char *cgiargs) 
{
    char *ptr;

    if (!strstr(uri, "cgi-bin")) {  /* Static content  中文：静态内容。 */ //line:netp:parseuri:isstatic
	/* 静态文件不使用 CGI 参数；在 URI 前加 '.' 得到相对路径。
	 * 例如 /images/a.gif -> ./images/a.gif，以服务器启动时的工作目录为基准。
	 */
	strcpy(cgiargs, "");                             //line:netp:parseuri:clearcgi
	strcpy(filename, ".");                           //line:netp:parseuri:beginconvert1
	strcat(filename, uri);                           //line:netp:parseuri:endconvert1
	/* 请求目录时补默认首页，例如 / -> ./home.html。 */
	if (uri[strlen(uri)-1] == '/')                   //line:netp:parseuri:slashcheck
	    strcat(filename, "home.html");               //line:netp:parseuri:appenddefault
	return 1;
    }
    else {  /* Dynamic content  中文：动态内容。 */                        //line:netp:parseuri:isdynamic
	/* 例如 /cgi-bin/adder?1&2：程序路径为 /cgi-bin/adder，参数字符串为 1&2。
	 * index(uri, '?') 找到第一个问号的位置，没有问号时返回 NULL。
	 */
	ptr = index(uri, '?');                           //line:netp:parseuri:beginextract
	if (ptr) {
	    /* 先复制问号后面的参数，再把问号改为字符串结束符。
	     * ptr+1 指向问号后的字符；*ptr='\0' 会直接截短原来的 uri。
	     */
	    strcpy(cgiargs, ptr+1);
	    *ptr = '\0';
	}
	else 
	    strcpy(cgiargs, "");                         //line:netp:parseuri:endextract
	/* 参数已经分离，文件名只保留 CGI 程序路径，例如 ./cgi-bin/adder。 */
	strcpy(filename, ".");                           //line:netp:parseuri:beginconvert2
	strcat(filename, uri);                           //line:netp:parseuri:endconvert2
	return 0;
    }
}
/* $end parse_uri */

/*
 * serve_static - copy a file back to the client 
 * 中文：先发送 HTTP 状态行和响应头，再把本地文件的原始字节作为响应体发送。
 * fd 是客户端连接；filename 是本地文件名；filesize 是文件字节数。
 */
/* $begin serve_static */
void serve_static(int fd, char *filename, int filesize)
{
    int srcfd;
    char *srcp, filetype[MAXLINE], buf[MAXBUF];

    /* Send response headers to client
     * HTTP 响应结构：状态行\r\n + 各响应头\r\n + 空行\r\n + 正文。
     * sprintf 仅在内存中生成字符串；Rio_writen 才会向连接写入数据。
     */
    get_filetype(filename, filetype);    //line:netp:servestatic:getfiletype
    sprintf(buf, "HTTP/1.0 200 OK\r\n"); //line:netp:servestatic:beginserve
    Rio_writen(fd, buf, strlen(buf));
    sprintf(buf, "Server: Tiny Web Server\r\n");
    Rio_writen(fd, buf, strlen(buf));
    /* 告诉客户端后续正文有多少字节，长度不包括状态行和响应头。 */
    sprintf(buf, "Content-length: %d\r\n", filesize);
    Rio_writen(fd, buf, strlen(buf));
    /* 最后的两个 \r\n 分别结束当前头字段、产生分隔响应头与正文的空行。 */
    sprintf(buf, "Content-type: %s\r\n\r\n", filetype);
    Rio_writen(fd, buf, strlen(buf));    //line:netp:servestatic:endserve

    /* Send response body to client
     * 以只读方式打开文件，将文件映射到进程地址空间，得到可读取的地址 srcp。
     * PROT_READ 表示只读映射；MAP_PRIVATE 表示私有映射，不把修改写回原文件。
     */
    srcfd = Open(filename, O_RDONLY, 0); //line:netp:servestatic:open
    srcp = Mmap(0, filesize, PROT_READ, MAP_PRIVATE, srcfd, 0); //line:netp:servestatic:mmap
    /* 映射建立后即使关闭文件描述符，映射仍然有效，直到 Munmap。 */
    Close(srcfd);                       //line:netp:servestatic:close
    /* 正文可能是图片等二进制数据，必须使用 filesize，不能使用 strlen(srcp)。
     * strlen 遇到 '\0' 就停止，而且映射的文件内容不保证以 '\0' 结尾。
     */
    Rio_writen(fd, srcp, filesize);     //line:netp:servestatic:write
    /* 释放文件映射，客户端连接仍由 main 负责关闭。 */
    Munmap(srcp, filesize);             //line:netp:servestatic:munmap
}

/*
 * get_filetype - derive file type from file name
 * 中文：根据文件名推断 MIME 类型，写入调用者的 filetype 缓冲区。
 * MIME 类型用于 Content-type，帮助浏览器决定如何解释正文。
 * 这里仅简单查找文件名中的子串，不检查文件真实内容，也不严格匹配扩展名。
 */
void get_filetype(char *filename, char *filetype) 
{
    if (strstr(filename, ".html"))
	strcpy(filetype, "text/html");
    else if (strstr(filename, ".gif"))
	strcpy(filetype, "image/gif");
    else if (strstr(filename, ".png"))
	strcpy(filetype, "image/png");
    else if (strstr(filename, ".jpg"))
	strcpy(filetype, "image/jpeg");
    else
	strcpy(filetype, "text/plain");
}  
/* $end serve_static */

/*
 * serve_dynamic - run a CGI program on behalf of the client
 * 中文：创建子进程运行 CGI 程序，由程序生成动态响应。
 * 父进程先发送状态行和 Server 头；CGI 程序负责剩余响应头、空行和正文。
 */
/* $begin serve_dynamic */
void serve_dynamic(int fd, char *filename, char *cgiargs) 
{
    /* 本实现不通过 argv 传递查询参数，而是通过环境变量 QUERY_STRING 传递。 */
    char buf[MAXLINE], *emptylist[] = { NULL };

    /* Return first part of HTTP response
     * 此处不发送结束响应头的空行，因为 CGI 程序还需要发送 Content-type 等头。
     */
    sprintf(buf, "HTTP/1.0 200 OK\r\n"); 
    Rio_writen(fd, buf, strlen(buf));
    sprintf(buf, "Server: Tiny Web Server\r\n");
    Rio_writen(fd, buf, strlen(buf));
  
    /* Fork 在子进程中返回 0，在父进程中返回子进程 PID。
     * 子进程继承打开的客户端连接，因此可以继续向同一客户端输出。
     */
    if (Fork() == 0) { /* Child  中文：子进程。 */ //line:netp:servedynamic:fork
	/* Real server would set all CGI vars here  中文：真正的服务器会在这里设置全部 CGI 环境变量。 */
	/* CGI 用 getenv("QUERY_STRING") 取得问号后的参数，例如 "1&2"。
	 * 最后一个参数 1 表示覆盖已有的同名环境变量。
	 */
	setenv("QUERY_STRING", cgiargs, 1); //line:netp:servedynamic:setenv
	/* 让标准输出描述符 1 指向客户端连接；CGI 向 stdout 输出的内容便会发给客户端。 */
	Dup2(fd, STDOUT_FILENO);         /* Redirect stdout to client  中文：将标准输出重定向到客户端连接。 */ //line:netp:servedynamic:dup2
	/* 用 CGI 程序替换子进程当前的程序映像；继承 environ 和已设置的标准输出。
	 * Execve 成功后不会返回，不是像普通函数调用一样执行完再回到这里。
	 */
	Execve(filename, emptylist, environ); /* Run CGI program  中文：运行 CGI 程序。 */ //line:netp:servedynamic:execve
    }
    /* 父进程阻塞等待 CGI 结束并回收它，避免留下僵尸进程。
     * 这里虽然 fork 了子进程，但父进程立即等待，因此 Tiny 仍然逐个处理连接。
     */
    Wait(NULL); /* Parent waits for and reaps child  中文：父进程等待并回收子进程。 */ //line:netp:servedynamic:wait
}
/* $end serve_dynamic */

/*
 * clienterror - returns an error message to the client
 * 中文：构造并发送一个 HTTP 错误响应，正文是用于展示错误的 HTML。
 * cause：错误关联的内容，如文件名或请求方法。
 * errnum：状态码字符串，如 "404"；shortmsg：简短描述，如 "Not found"。
 * longmsg：更详细的错误原因。这个函数只发响应，不关闭连接。
 */
/* $begin clienterror */
void clienterror(int fd, char *cause, char *errnum, 
		 char *shortmsg, char *longmsg) 
{
    char buf[MAXLINE];

    /* Print the HTTP response headers
     * 先发送状态行，再发送 Content-type；末尾空行表示后面开始发送 HTML 正文。
     */
    sprintf(buf, "HTTP/1.0 %s %s\r\n", errnum, shortmsg);
    Rio_writen(fd, buf, strlen(buf));
    sprintf(buf, "Content-type: text/html\r\n\r\n");
    Rio_writen(fd, buf, strlen(buf));

    /* Print the HTTP response body
     * 分多次写出 HTML 片段，在 TCP 字节流中仍是连续的响应正文。
     * 这里没有 Content-length，客户端通过连接关闭判断正文结束。
     */
    sprintf(buf, "<html><title>Tiny Error</title>");
    Rio_writen(fd, buf, strlen(buf));
    sprintf(buf, "<body bgcolor=""ffffff"">\r\n");
    Rio_writen(fd, buf, strlen(buf));
    sprintf(buf, "%s: %s\r\n", errnum, shortmsg);
    Rio_writen(fd, buf, strlen(buf));
    sprintf(buf, "<p>%s: %s\r\n", longmsg, cause);
    Rio_writen(fd, buf, strlen(buf));
    sprintf(buf, "<hr><em>The Tiny Web server</em>\r\n");
    Rio_writen(fd, buf, strlen(buf));
}
/* $end clienterror */
