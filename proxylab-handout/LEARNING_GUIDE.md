# Proxy Lab · 把网络、并发和缓存连起来

[总路线](../README.md) · [工具箱](../learning/TOOLBOX.md) · 原始依据：[README](README)、[proxylab.pdf](proxylab.pdf)、`proxy.c`、`csapp.c`、`tiny/tiny.c`。

保留当前起始程序，由你完成顺序代理、并发代理与缓存。先打通一条可靠的字节路径，再增加并发与共享状态；最终仍按原三个部分验收。

## 热身：先直连 Tiny

在终端 A，从 `proxylab-handout` 出发：

```sh
cd tiny
make
./tiny 18080
```

终端 B 进入 `proxylab-handout`：

```sh
curl --noproxy '*' --max-time 5 -v http://127.0.0.1:18080/home.html
```

18080 是示例端口，若占用则换一个并同步修改命令。Tiny 持续运行，观察请求完毕后用该终端的 Ctrl-C 结束。过关标准：分清状态行、请求/响应头、空行和 body，确认源服务器本身正常。

## 关键知识

| 概念 / Concept | 为什么影响正确性 |
|---|---|
| 字节流 / Byte stream | TCP 不保证一次 read 就是一条完整消息 |
| RIO / Robust I/O | 处理短读、短写；文本头与二进制 body 用不同观察方法 |
| 绝对 URI / Absolute URI | 客户端发给代理的 URI 要解析为主机、端口、路径 |
| 描述符所有权 / Descriptor ownership | 谁负责关闭客户端和源服务器连接？ |
| 共享状态 / Shared state | 缓存访问和 LRU 更新可能同时发生 |
| 资源上限 / Resource limits | 连接、线程、缓存空间都不能无界增长 |

网络错误也可能发生在 CS:APP 的包装函数中：阅读会调用退出路径的包装函数，避免单个连接失败让长期运行的代理进程退出。

## 阶段 1：顺序代理，先文本再二进制

先读 `tiny.c` 和 RIO 接口。在笔记画出“客户端 → 代理 → 源服务器 → 代理 → 客户端”，给每个箭头标出传输内容、结束条件、错误处理和关闭责任。

依次完成监听与接受、请求行解析、请求头处理、源服务器连接、响应转发。GET 的上游请求按讲义使用 HTTP/1.0，确保头行用 CRLF 并以空行结束。

请求头规则以 PDF 为准：必须提供 Host，已有 Host 按讲义保留；Connection 和 Proxy-Connection 使用 close，避免重复冲突；User-Agent 可使用起始代码提供的值。其他需要保留的头不要无故丢弃。

你实现后在终端 C 从 lab 目录运行：

```sh
make
./proxy 18081
```

终端 B 验证（Tiny 仍在 18080）：

```sh
curl --noproxy '' --max-time 5 -v -x http://127.0.0.1:18081 http://127.0.0.1:18080/home.html
curl --noproxy '*' --max-time 5 -o /tmp/csapp-direct.jpg http://127.0.0.1:18080/godzilla.jpg
curl --noproxy '' --max-time 5 -x http://127.0.0.1:18081 -o /tmp/csapp-proxied.jpg http://127.0.0.1:18080/godzilla.jpg
cmp /tmp/csapp-direct.jpg /tmp/csapp-proxied.jpg
```

`--noproxy ''` 明确要求经过代理，避免 localhost 被环境变量绕过。过关标准：文本和图片都完整；cmp 无差异；异常请求和断开连接后代理仍能服务下一次请求。

<details>
<summary>提示 1：页面能打开，图片却损坏</summary>

响应 body 可能包含零字节，不能用 strlen 决定长度。记录每次读取返回的字节数，按这些字节转发，并处理短写。不要把二进制响应当零终止字符串。

</details>

## 阶段 2：并发，先明确私有状态

读讲义允许的并发模型，选择线程等策略。标出每个请求私有的 URI、缓冲区、RIO 状态和连接 fd；确认创建任务时传递的参数不会被下一次 accept 覆盖。

用一个慢请求和一个快请求同时访问：快请求不应被慢请求阻塞。还要检查线程回收/分离、各错误路径的 fd 关闭和长期资源占用。

过关标准：并发请求内容正确，慢请求不阻塞所有服务；连接异常不会终止代理。仅“能开两个终端”不能证明并发实现可靠。

<details>
<summary>提示 2：并发时两个请求混在一起</summary>

检查任务参数是否指向 accept 循环反复写入的局部变量；检查缓冲区或解析状态是否被共享。为每个请求画独立的对象生存期，再检查什么时候可以释放。

</details>

## 阶段 3：缓存，在加锁前设计协议

按原上限：`MAX_CACHE_SIZE=1049000`、`MAX_OBJECT_SIZE=102400`。先决定缓存键、对象长度、查找/插入/替换流程，再定义同步规则。缓存键要区分源服务器端口和路径，不只保存文件名。

只把完整接收且符合大小限制的对象插入；对象超过上限仍应正常转发。命中后的 LRU 更新本身也是共享状态修改；不能因“只读对象数据”就忽略替换元数据竞态。

测试：同 URL 重复访问、不同端口同路径、对象恰在上限附近、超过上限、多请求同时命中/插入、读者与驱逐交错。

过关标准：缓存内容与直连字节一致，空间计数可靠，驱逐后无悬空读取；能解释慢网络 I/O 期间是否持有全局锁，以及这样做对吞吐量的影响。

<details>
<summary>提示 3：缓存命中后偶尔崩溃</summary>

画出“读者取得指针 → 驱逐者释放对象 → 读者发送”的顺序。保护查找动作本身不一定保护对象整个使用期；需要证明读取期间对象不会失效。

</details>

## 完整验收与进阶

本仓库 `driver.sh` 会先 `killall` 同名的 proxy、tiny、nop-server.py，适合独立实验环境；运行前结束你自己的手动服务，并确保没有其他同名任务需要保留。它依赖 curl、netstat 等工具；`nop-server.py` 的代码兼容 Python 3，但首行写的是 `/usr/bin/python`，直接执行时需确保这个解释器入口存在。

在独立实验环境准备好依赖后：

```sh
make
./driver.sh
```

检查 Basic、Concurrency、Cache 三部分原测试，再补二进制、异常输入、资源回收和缓存交错验证。手工 curl 通过不代表综合驱动已通过；缺依赖时先记录环境问题。

进阶：画并发缓存的锁顺序和对象生存期；观察慢客户端对资源占用的影响；说明本实验的简化 HTTP 与完整代理协议的差距。你最终应能把一次请求从接受到清理完整讲出来。
