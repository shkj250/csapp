# The Attack Lab: Understanding Buffer Overflow Bugs
# Attack Lab：理解缓冲区溢出漏洞

**15-213, Fall 20xx** — **15-213，20xx 年秋季学期**  
**Assigned: Tue, Sept. 29** — **布置日期：9 月 29 日（星期二）**  
**Due: Thu, Oct. 8, 11:59PM EDT** — **截止日期：10 月 8 日（星期四），美国东部夏令时 23:59**  
**Last Possible Time to Turn in: Sun, Oct. 11, 11:59PM EDT** — **最后提交期限：10 月 11 日（星期日），美国东部夏令时 23:59**

> 对照说明：以下按原文顺序排列，先英文、后中文；保留代码、地址、寄存器、函数名及命令。译文覆盖用户所提供的 10 页文字；原文件没有附录 A、B 和图 3 的具体内容。

## 1 Introduction — 引言

**EN** This assignment involves generating a total of five attacks on two programs having different security vulnerabilities. Outcomes you will gain from this lab include:

**中** 本次作业要求针对两个具有不同安全漏洞的程序，设计总共五次攻击。完成实验后，你将获得以下收获：

- **EN** You will learn different ways that attackers can exploit security vulnerabilities when programs do not safeguard themselves well enough against buffer overflows.  
  **中** 了解当程序未充分防范缓冲区溢出时，攻击者可能利用安全漏洞的不同方式。
- **EN** Through this, you will get a better understanding of how to write programs that are more secure, as well as some of the features provided by compilers and operating systems to make programs less vulnerable.  
  **中** 进一步理解如何编写更安全的程序，以及编译器和操作系统提供的、用于降低程序受攻击风险的安全机制。
- **EN** You will gain a deeper understanding of the stack and parameter-passing mechanisms of x86-64 machine code.  
  **中** 更深入地理解 x86-64 机器码中的栈和参数传递机制。
- **EN** You will gain a deeper understanding of how x86-64 instructions are encoded.  
  **中** 更深入地理解 x86-64 指令如何编码成机器码。
- **EN** You will gain more experience with debugging tools such as GDB and OBJDUMP.  
  **中** 积累使用 GDB、OBJDUMP 等调试和分析工具的经验。

**EN — Note:** In this lab, you will gain firsthand experience with methods used to exploit security weaknesses in operating systems and network servers. Our purpose is to help you learn about the runtime operation of programs and to understand the nature of these security weaknesses so that you can avoid them when you write system code. We do not condone the use of any other form of attack to gain unauthorized access to any system resources.

**中 — 注意：** 本实验将让你亲自体验利用操作系统及网络服务器安全缺陷的方法。其目的在于帮助你理解程序的运行机制与这些安全漏洞的本质，以便将来编写系统代码时避免此类问题。我们不赞成通过其他形式的攻击未经授权地访问任何系统资源。

**EN** You will want to study Sections 3.10.3 and 3.10.4 of the CS:APP3e book as reference material for this lab.

**中** 建议阅读《深入理解计算机系统》第三版（CS:APP3e）的第 3.10.3 节和第 3.10.4 节，作为本实验的参考资料。

## 2 Logistics — 实验安排

**EN** As usual, this is an individual project. You will generate attacks for target programs that are custom generated for you.

**中** 与以往一样，这是一个个人独立完成的项目。你将针对为自己专门生成的目标程序设计攻击。

### 2.1 Getting Files — 获取实验文件

**EN** You can obtain your files by pointing your Web browser at:

**中** 在浏览器中访问以下地址即可获取文件：

```text
http://$Attacklab::SERVER_NAME:15513/
```

**EN — INSTRUCTOR:** `$Attacklab::SERVER_NAME` is the machine that runs the attacklab servers. You define it in `attacklab/Attacklab.pm` and in `attacklab/src/build/driverhdrs.h`.

**中 — 教师注意：** `$Attacklab::SERVER_NAME` 指运行 Attack Lab 服务端的机器。需要在 `attacklab/Attacklab.pm` 和 `attacklab/src/build/driverhdrs.h` 中进行定义。

**EN** The server will build your files and return them to your browser in a tar file called `targetk.tar`, where `k` is the unique number of your target programs.

**中** 服务器会生成你的实验文件，并通过浏览器返回名为 `targetk.tar` 的压缩包，其中 `k` 是目标程序的唯一编号。

**EN — Note:** It takes a few seconds to build and download your target, so please be patient.

**中 — 注意：** 生成和下载目标程序可能需要几秒钟。

**EN** Save the `targetk.tar` file in a (protected) Linux directory in which you plan to do your work. Then give the command: `tar -xvf targetk.tar`. This will extract a directory `targetk` containing the files described below.

**中** 将 `targetk.tar` 保存到准备用于实验的、具有适当访问保护的 Linux 目录中，然后执行 `tar -xvf targetk.tar`。解压后会得到 `targetk` 目录，其中包含下面列出的文件。

**EN** You should only download one set of files. If for some reason you download multiple targets, choose one target to work on and delete the rest.

**中** 只应下载一套实验文件。如果因某种原因下载了多套目标程序，请选择其中一套进行实验，并删除其他套。

**EN — Warning:** If you expand your `targetk.tar` on a PC, by using a utility such as Winzip, or letting your browser do the extraction, you’ll risk resetting permission bits on the executable files.

**中 — 警告：** 如果使用 WinZip 等 PC 工具或浏览器直接解压 `targetk.tar`，可能导致可执行文件的权限位被重置。

**EN** The files in `targetk` include:

**中** `targetk` 目录包括以下文件：

- **EN** `README.txt`: A file describing the contents of the directory.  
  **中** `README.txt`：目录内容说明。
- **EN** `ctarget`: An executable program vulnerable to code-injection attacks.  
  **中** `ctarget`：存在代码注入漏洞的可执行程序。
- **EN** `rtarget`: An executable program vulnerable to return-oriented-programming attacks.  
  **中** `rtarget`：存在返回导向编程（ROP）攻击漏洞的可执行程序。
- **EN** `cookie.txt`: An 8-digit hex code that you will use as a unique identifier in your attacks.  
  **中** `cookie.txt`：包含一个 8 位十六进制数，将作为本次攻击中的唯一标识。
- **EN** `farm.c`: The source code of your target’s “gadget farm,” which you will use in generating return-oriented programming attacks.  
  **中** `farm.c`：目标程序中“gadget 农场”的源代码，将在构造 ROP 攻击时使用。
- **EN** `hex2raw`: A utility to generate attack strings.  
  **中** `hex2raw`：用于生成攻击输入字节串的工具。

**EN** In the following instructions, we will assume that you have copied the files to a protected local directory, and that you are executing the programs in that local directory.

**中** 以下说明假定你已将实验文件放入一个受保护的本地目录，并在该目录中运行这些程序。

### 2.2 Important Points — 重要规则

**EN** Here is a summary of some important rules regarding valid solutions for this lab. These points will not make much sense when you read this document for the first time. They are presented here as a central reference of rules once you get started.

**中** 下面概述本实验有效解答必须遵守的一些重要规则。第一次阅读时，你可能还不理解它们；等真正开始实验后，可以将这里作为统一的规则参考。

- **EN** You must do the assignment on a machine that is similar to the one that generated your targets.  
  **中** 必须在与生成目标程序时所用环境相近的机器上完成实验。
- **EN** Your solutions may not use attacks to circumvent the validation code in the programs. Specifically, any address you incorporate into an attack string for use by a `ret` instruction should be to one of the following destinations:  
  **中** 解答不得通过攻击来绕过程序中的验证代码。特别是，攻击字符串中供 `ret` 指令使用的地址，只能指向以下三类位置：
  - **EN** The addresses for functions `touch1`, `touch2`, or `touch3`.  
    **中** 函数 `touch1`、`touch2` 或 `touch3` 的地址。
  - **EN** The address of your injected code.  
    **中** 你注入的代码的地址。
  - **EN** The address of one of your gadgets from the gadget farm.  
    **中** 你从 gadget farm 中选取的某个 gadget 的地址。
- **EN** You may only construct gadgets from file `rtarget` with addresses ranging between those for functions `start_farm` and `end_farm`.  
  **中** 只能使用 `rtarget` 中 `start_farm` 和 `end_farm` 两个函数地址之间的代码构造 gadget。

## 3 Target Programs — 目标程序

**EN** Both CTARGET and RTARGET read strings from standard input. They do so with the function `getbuf` defined below:

**中** CTARGET 和 RTARGET 都从标准输入读取字符串。它们使用下方定义的 `getbuf` 函数完成读取：

```c
unsigned getbuf()
{
    char buf[BUFFER_SIZE];
    Gets(buf);
    return 1;
}
```

**EN** The function `Gets` is similar to the standard library function `gets`—it reads a string from standard input (terminated by `\n` or end-of-file) and stores it (along with a null terminator) at the specified destination. In this code, you can see that the destination is an array `buf`, declared as having `BUFFER_SIZE` bytes. At the time your targets were generated, `BUFFER_SIZE` was a compile-time constant specific to your version of the programs.

**中** `Gets` 类似于标准库中的 `gets`：它从标准输入读取一个字符串，以换行符 `\n` 或文件结束符为输入终止条件，并把字符串连同末尾的空字符 `\0` 存放到指定位置。这里的目标位置是一个名叫 `buf`、大小为 `BUFFER_SIZE` 字节的数组。目标程序生成时，`BUFFER_SIZE` 就被确定为与你的程序版本对应的编译期常量。

**EN** Functions `Gets()` and `gets()` have no way to determine whether their destination buffers are large enough to store the string they read. They simply copy sequences of bytes, possibly overrunning the bounds of the storage allocated at the destinations.

**中** `Gets()` 和 `gets()` 无法判断目标缓冲区是否足够容纳读入的字符串；它们只会复制字节序列，因此可能写出目标缓冲区的边界。

**EN** If the string typed by the user and read by `getbuf` is sufficiently short, it is clear that `getbuf` will return 1, as shown by the following execution examples:

**中** 如果通过 `getbuf` 读取的输入字符串足够短，那么函数会正常返回 `1`，例如：

```text
unix> ./ctarget
Cookie: 0x1a7dd803
Type string: Keep it short!
No exploit. Getbuf returned 0x1
Normal return
```

**EN** Typically an error occurs if you type a long string:

**中** 如果输入过长，通常会发生错误：

```text
unix> ./ctarget
Cookie: 0x1a7dd803
Type string: This is not a very interesting string, but it has the property ...
Ouch!: You caused a segmentation fault!
Better luck next time
```

**EN** (Note that the value of the cookie shown will differ from yours.) Program RTARGET will have the same behavior. As the error message indicates, overrunning the buffer typically causes the program state to be corrupted, leading to a memory access error. Your task is to be more clever with the strings you feed CTARGET and RTARGET so that they do more interesting things. These are called exploit strings.

**中** （注意：示例中的 cookie 与你的 cookie 不同。）RTARGET 也会表现出类似行为。错误信息表明，缓冲区越界通常会破坏程序状态，并引发内存访问错误。你的任务是精心构造提供给 CTARGET 和 RTARGET 的输入，让程序发生更有针对性的行为。这些输入称为 **exploit strings（漏洞利用字符串）**。

**EN** Both CTARGET and RTARGET take several different command line arguments:

**中** CTARGET 和 RTARGET 支持以下命令行参数：

```text
-h       Print list of possible command line arguments
         显示可用命令行参数列表
-q       Don't send results to the grading server
         不向评分服务器发送结果
-i FILE  Supply input from a file, rather than from standard input
         从文件 FILE 读取输入，而不是从标准输入读取
```

**EN** Your exploit strings will typically contain byte values that do not correspond to the ASCII values for printing characters. The program HEX2RAW will enable you to generate these raw strings. See Appendix A for more information on how to use HEX2RAW.

**中** 攻击字符串通常包含无法用可打印 ASCII 字符直接表示的字节值。`HEX2RAW` 工具可以帮助生成这样的原始字节串。更多使用方法见原文附录 A。

**EN — Important points:**

**中 — 重要注意事项：**

- **EN** Your exploit string must not contain byte value `0x0a` at any intermediate position, since this is the ASCII code for newline (`\n`). When Gets encounters this byte, it will assume you intended to terminate the string.  
  **中** 攻击字符串的中间位置不能包含字节 `0x0a`，因为它表示换行符 `\n`；`Gets` 遇到它时会认为字符串输入已结束。
- **EN** HEX2RAW expects two-digit hex values separated by one or more white spaces. So if you want to create a byte with a hex value of 0, you need to write it as `00`. To create the word `0xdeadbeef` you should pass `ef be ad de` to HEX2RAW (note the reversal required for little-endian byte ordering).  
  **中** `HEX2RAW` 要求输入两位十六进制数，并以一个或多个空白字符分隔。例如，零字节要写成 `00`。如果要生成字（word）`0xdeadbeef`，应传入 `ef be ad de`，注意小端字节序要求将字节顺序反过来。

**EN** When you have correctly solved one of the levels, your target program will automatically send a notification to the grading server. For example:

**中** 正确完成某一关后，目标程序会自动向评分服务器发送通知。例如：

```text
unix> ./hex2raw < ctarget.l2.txt | ./ctarget
Cookie: 0x1a7dd803
Type string:Touch2!: You called touch2(0x1a7dd803)
Valid solution for level 2 with target ctarget
PASSED: Sent exploit string to server to be validated.
NICE JOB!
```

**EN — Figure 1: Summary of attack lab phases**  
**中 — 图 1：Attack Lab 各阶段概览**

| Phase 阶段 | Program 程序 | Level 关卡 | Method 方法 | Function 目标函数 | Points 分值 |
|---|---|---|---|---|---|
| 1 | CTARGET | 1 | CI | `touch1` | 10 |
| 2 | CTARGET | 2 | CI | `touch2` | 25 |
| 3 | CTARGET | 3 | CI | `touch3` | 25 |
| 4 | RTARGET | 2 | ROP | `touch2` | 35 |
| 5 | RTARGET | 3 | ROP | `touch3` | 5 |

**CI: Code injection — 代码注入；ROP: Return-oriented programming — 返回导向编程。**

**EN** The server will test your exploit string to make sure it really works, and it will update the Attacklab scoreboard page indicating that your userid (listed by your target number for anonymity) has completed this phase.

**中** 服务器会测试你的攻击字符串是否真正有效，并更新 Attack Lab 排行榜，标记你的用户（为匿名起见以目标编号展示）已完成相应关卡。

**EN** You can view the scoreboard by pointing your Web browser at `http://$Attacklab::SERVER_NAME:15513/scoreboard`.

**中** 可以在浏览器中访问 `http://$Attacklab::SERVER_NAME:15513/scoreboard` 查看排行榜。

**EN** Unlike the Bomb Lab, there is no penalty for making mistakes in this lab. Feel free to fire away at CTARGET and RTARGET with any strings you like.

**中** 与 Bomb Lab 不同，本实验不会因尝试失败而扣分。可以自由向 CTARGET 和 RTARGET 输入各种字符串进行实验。

**EN — IMPORTANT NOTE:** You can work on your solution on any Linux machine, but in order to submit your solution, you will need to be running on one of the following machines: **INSTRUCTOR:** Insert the list of the legal domain names that you established in `buflab/src/config.c`.

**中 — 重要说明：** 可以在任何 Linux 机器上研究解法，但正式提交时必须在课程规定的机器上运行。原文这里包含教师占位说明：需要填入在 `buflab/src/config.c` 中配置的合法域名列表。

**EN** Figure 1 summarizes the five phases of the lab. As can be seen, the first three involve code-injection (CI) attacks on CTARGET, while the last two involve return-oriented-programming (ROP) attacks on RTARGET.

**中** 图 1 总结了实验的五个阶段。前三个阶段针对 CTARGET 进行代码注入（CI）攻击，后两个阶段针对 RTARGET 进行返回导向编程（ROP）攻击。

## 4 Part I: Code Injection Attacks — 第一部分：代码注入攻击

**EN** For the first three phases, your exploit strings will attack CTARGET. This program is set up in a way that the stack positions will be consistent from one run to the next and so that data on the stack can be treated as executable code. These features make the program vulnerable to attacks where the exploit strings contain the byte encodings of executable code.

**中** 在前三个阶段，你将利用构造的攻击字符串攻击 CTARGET。该程序经过特殊配置，使栈地址在各次运行中保持一致，并允许把栈中的数据当作可执行代码执行。因此，攻击字符串可以包含可执行指令的机器码字节，以此利用程序漏洞。

### 4.1 Level 1 — 第一关

**EN** For Phase 1, you will not inject new code. Instead, your exploit string will redirect the program to execute an existing procedure.

**中** 第一阶段不需要注入新代码。你的攻击字符串只需要改变程序控制流，使其执行一个原本就存在的函数。

**EN** Function `getbuf` is called within CTARGET by a function `test` having the following C code:

**中** CTARGET 中的 `test` 函数调用 `getbuf`。`test` 的 C 代码如下：

```c
void test()
{
    int val;
    val = getbuf();
    printf("No exploit. Getbuf returned 0x%x\n", val);
}
```

**EN** When `getbuf` executes its return statement (line 5 of `getbuf`), the program ordinarily resumes execution within function `test` (at line 5 of this function). We want to change this behavior. Within the file `ctarget`, there is code for a function `touch1` having the following C representation:

**中** 正常情况下，当 `getbuf` 执行返回语句（`getbuf` 的第 5 行）时，程序会返回 `test` 函数，并从 `test` 的第 5 行继续执行。我们的目标就是改变这一行为。`ctarget` 中还存在一个 `touch1` 函数，其 C 代码等价于：

```c
void touch1()
{
    vlevel = 1; /* Part of validation protocol */
    printf("Touch1!: You called touch1()\n");
    validate(1);
    exit(0);
}
```

**EN** Your task is to get CTARGET to execute the code for `touch1` when `getbuf` executes its return statement, rather than returning to `test`. Note that your exploit string may also corrupt parts of the stack not directly related to this stage, but this will not cause a problem, since `touch1` causes the program to exit directly.

**中** 你的任务是让 CTARGET 在 `getbuf` 返回时改为执行 `touch1`，而不是回到 `test`。请注意，攻击字符串可能同时破坏栈上与本关无关的其他数据，但这不会妨碍本关成功，因为 `touch1` 会直接结束程序。

**EN — Some Advice:**

**中 — 一些建议：**

- **EN** All the information you need to devise your exploit string for this level can be determined by examining a disassembled version of CTARGET. Use `objdump -d` to get this disassembled version.  
  **中** 本关所需的信息都可以通过分析 CTARGET 的反汇编结果获得。使用 `objdump -d` 生成反汇编。
- **EN** The idea is to position a byte representation of the starting address for `touch1` so that the `ret` instruction at the end of the code for `getbuf` will transfer control to `touch1`.  
  **中** 基本思路是将 `touch1` 入口地址的字节表示放在合适位置，使 `getbuf` 末尾的 `ret` 指令把控制流转移到 `touch1`。
- **EN** Be careful about byte ordering.  
  **中** 注意字节序。
- **EN** You might want to use GDB to step the program through the last few instructions of `getbuf` to make sure it is doing the right thing.  
  **中** 可以使用 GDB 单步执行 `getbuf` 的最后几条指令，确认程序按预期跳转。
- **EN** The placement of `buf` within the stack frame for `getbuf` depends on the value of compile-time constant `BUFFER_SIZE`, as well as the allocation strategy used by GCC. You will need to examine the disassembled code to determine its position.  
  **中** `buf` 在 `getbuf` 栈帧中的位置取决于编译期常量 `BUFFER_SIZE` 和 GCC 的栈空间分配策略。需要分析反汇编，才能确定实际位置。

### 4.2 Level 2 — 第二关

**EN** Phase 2 involves injecting a small amount of code as part of your exploit string.

**中** 第二阶段需要在攻击字符串中放入少量注入代码。

**EN** Within the file `ctarget` there is code for a function `touch2` having the following C representation:

**中** `ctarget` 中存在 `touch2` 函数，其 C 代码等价于：

```c
void touch2(unsigned val)
{
    vlevel = 2; /* Part of validation protocol */
    if (val == cookie) {
        printf("Touch2!: You called touch2(0x%.8x)\n", val);
        validate(2);
    } else {
        printf("Misfire: You called touch2(0x%.8x)\n", val);
        fail(2);
    }
    exit(0);
}
```

**EN** Your task is to get CTARGET to execute the code for `touch2` rather than returning to `test`. In this case, however, you must make it appear to `touch2` as if you have passed your cookie as its argument.

**中** 你的任务是让 CTARGET 转而执行 `touch2`，而不是返回 `test`。但与第一关不同，这一次必须让 `touch2` 看起来接收到了你的 cookie 作为参数。

**EN — Some Advice:**

**中 — 一些建议：**

- **EN** You will want to position a byte representation of the address of your injected code in such a way that `ret` instruction at the end of the code for `getbuf` will transfer control to it.  
  **中** 需要把注入代码的地址放在合适的位置，让 `getbuf` 最后的 `ret` 跳转到注入代码。
- **EN** Recall that the first argument to a function is passed in register `%rdi`.  
  **中** 回忆一下：函数的第一个参数通过 `%rdi` 寄存器传递。
- **EN** Your injected code should set the register to your cookie, and then use a `ret` instruction to transfer control to the first instruction in `touch2`.  
  **中** 注入代码应将相应寄存器设置为你的 cookie，然后用 `ret` 指令把控制流转移到 `touch2` 的第一条指令。
- **EN** Do not attempt to use `jmp` or `call` instructions in your exploit code. The encodings of destination addresses for these instructions are difficult to formulate. Use `ret` instructions for all transfers of control, even when you are not returning from a call.  
  **中** 不要尝试在攻击代码中使用 `jmp` 或 `call`，因为它们所需目标地址的机器码编码较难构造。所有控制流转移都使用 `ret`，即使这并不是真正的函数返回。
- **EN** See the discussion in Appendix B on how to use tools to generate the byte-level representations of instruction sequences.  
  **中** 关于如何用工具生成指令序列的机器码字节表示，请参考原文附录 B。

### 4.3 Level 3 — 第三关

**EN** Phase 3 also involves a code injection attack, but passing a string as argument.

**中** 第三阶段仍然使用代码注入，但这次需要传入一个字符串作为参数。

**EN** Within the file `ctarget` there is code for functions `hexmatch` and `touch3` having the following C representations:

**中** `ctarget` 中的 `hexmatch` 和 `touch3` 函数对应以下 C 代码：

```c
/* Compare string to hex represention of unsigned value */
int hexmatch(unsigned val, char *sval)
{
    char cbuf[110];
    /* Make position of check string unpredictable */
    char *s = cbuf + random() % 100;
    sprintf(s, "%.8x", val);
    return strncmp(sval, s, 9) == 0;
}

void touch3(char *sval)
{
    vlevel = 3; /* Part of validation protocol */
    if (hexmatch(cookie, sval)) {
        printf("Touch3!: You called touch3(\"%s\")\n", sval);
        validate(3);
    } else {
        printf("Misfire: You called touch3(\"%s\")\n", sval);
        fail(3);
    }
    exit(0);
}
```

**EN** Your task is to get CTARGET to execute the code for `touch3` rather than returning to `test`. You must make it appear to `touch3` as if you have passed a string representation of your cookie as its argument.

**中** 你要让 CTARGET 在应当返回 `test` 时执行 `touch3`，并让 `touch3` 以为接收到的是 cookie 的字符串形式。

**EN — Some Advice:**

**中 — 一些建议：**

- **EN** You will need to include a string representation of your cookie in your exploit string. The string should consist of the eight hexadecimal digits (ordered from most to least significant) without a leading `0x`.  
  **中** 攻击字符串中必须包含 cookie 的字符串表示：按从最高有效位到最低有效位的顺序写出 8 个十六进制数字，前面不加 `0x`。
- **EN** Recall that a string is represented in C as a sequence of bytes followed by a byte with value 0. Type `man ascii` on any Linux machine to see the byte representations of the characters you need.  
  **中** C 字符串由一串字节和末尾值为 `0` 的字节组成。可以在 Linux 中输入 `man ascii`，查看所需字符对应的字节值。
- **EN** Your injected code should set register `%rdi` to the address of this string.  
  **中** 注入代码应把 `%rdi` 设置为该字符串的地址。
- **EN** When functions `hexmatch` and `strncmp` are called, they push data onto the stack, overwriting portions of memory that held the buffer used by `getbuf`. As a result, you will need to be careful where you place the string representation of your cookie.  
  **中** 调用 `hexmatch` 和 `strncmp` 时，这些函数会向栈中写入数据，覆盖先前 `getbuf` 缓冲区所占用的部分内存。因此必须仔细选择 cookie 字符串的存放位置。

## 5 Part II: Return-Oriented Programming — 第二部分：返回导向编程

**EN** Performing code-injection attacks on program RTARGET is much more difficult than it is for CTARGET, because it uses two techniques to thwart such attacks:

**中** 对 RTARGET 进行代码注入，比攻击 CTARGET 困难得多，因为 RTARGET 采用了两种防御技术：

- **EN** It uses randomization so that the stack positions differ from one run to another. This makes it impossible to determine where your injected code will be located.  
  **中** 使用随机化技术，让每次运行时的栈位置发生变化，因此无法事先确定注入代码位于何处。
- **EN** It marks the section of memory holding the stack as nonexecutable, so even if you could set the program counter to the start of your injected code, the program would fail with a segmentation fault.  
  **中** 将栈所在内存区域标记为不可执行。即使把程序计数器设置为注入代码的起始地址，执行时也会触发段错误。

**EN** Fortunately, clever people have devised strategies for getting useful things done in a program by executing existing code, rather than injecting new code. The most general form of this is referred to as return-oriented programming (ROP) [1, 2]. The strategy with ROP is to identify byte sequences within an existing program that consist of one or more instructions followed by the instruction `ret`. Such a segment is referred to as a gadget.

**中** 好在研究人员提出了一种不注入新代码、而利用程序已有代码完成特定操作的方法。这类方法中最典型的是返回导向编程（Return-Oriented Programming，ROP）[1, 2]。ROP 会在现有程序中寻找由一条或多条指令加上一条 `ret` 指令构成的字节序列，这样的短代码片段叫作 **gadget**。

**EN — Figure 2:** Setting up sequence of gadgets for execution. Byte value `0xc3` encodes the `ret` instruction.

**中 — 图 2：** 在栈上布置依次执行的 gadget 地址。字节 `0xc3` 是 `ret` 指令的机器码。

```text
Stack (conceptual diagram / 栈的概念示意)
%rsp -> [ Address of Gadget 1 / Gadget 1 地址 ]
        [ Address of Gadget 2 / Gadget 2 地址 ]
        [          ...                    ]
        [ Address of Gadget n / Gadget n 地址 ]
Each gadget ends with 'ret' (machine byte c3).
每个 gadget 均以 ret（机器码 c3）结尾。
```

**EN** Figure 2 illustrates how the stack can be set up to execute a sequence of `n` gadgets. In this figure, the stack contains a sequence of gadget addresses. Each gadget consists of a series of instruction bytes, with the final one being `0xc3`, encoding the `ret` instruction. When the program executes a `ret` instruction starting with this configuration, it will initiate a chain of gadget executions, with the `ret` instruction at the end of each gadget causing the program to jump to the beginning of the next.

**中** 图 2 说明如何布置栈，使程序连续执行 `n` 个 gadget。栈中依次保存各个 gadget 的地址。每个 gadget 都由一段指令字节组成，最后一个字节为代表 `ret` 的 `0xc3`。在这种布局下，当程序开始执行 `ret` 后，就会触发 gadget 执行链：一个 gadget 末尾的 `ret` 会使程序跳转到下一个 gadget 的开头。

**EN** A gadget can make use of code corresponding to assembly-language statements generated by the compiler, especially ones at the ends of functions. In practice, there may be some useful gadgets of this form, but not enough to implement many important operations. For example, it is highly unlikely that a compiled function would have `popq %rdi` as its last instruction before `ret`. Fortunately, with a byte-oriented instruction set, such as x86-64, a gadget can often be found by extracting patterns from other parts of the instruction byte sequence.

**中** gadget 可以直接采用编译器生成的汇编指令，特别是函数末尾的指令片段。不过实际程序中这类有用片段可能数量不足，难以完成多种重要操作。例如，编译后的函数几乎不可能恰好以 `popq %rdi`、`ret` 两条指令结尾。幸运的是，x86-64 属于按字节编码的指令集，因而有时可以从其他指令的机器码字节序列中截取出有效的 gadget。

**EN** For example, one version of `rtarget` contains code generated for the following C function:

**中** 例如，某个版本的 `rtarget` 包含如下 C 函数生成的代码：

```c
void setval_210(unsigned *p)
{
    *p = 3347663060U;
}
```

**EN** The chances of this function being useful for attacking a system seem pretty slim. But, the disassembled machine code for this function shows an interesting byte sequence:

**中** 这个函数本身似乎不太可能对攻击有什么帮助，但反汇编后可以观察到一段有意思的字节序列：

```asm
0000000000400f15 <setval_210>:
400f15: c7 07 d4 48 89 c7    movl $0xc78948d4,(%rdi)
400f1b: c3                   retq
```

**EN** The byte sequence `48 89 c7` encodes the instruction `movq %rax, %rdi`. (See Figure 3A for the encodings of useful `movq` instructions.) This sequence is followed by byte value `c3`, which encodes the `ret` instruction. The function starts at address `0x400f15`, and the sequence starts on the fourth byte of the function. Thus, this code contains a gadget, having a starting address of `0x400f18`, that will copy the 64-bit value in register `%rax` to register `%rdi`.

**中** 字节序列 `48 89 c7` 可以解码为指令 `movq %rax, %rdi`（实用 `movq` 指令的编码参见原文图 3A）。紧随其后的是表示 `ret` 的字节 `c3`。函数入口地址为 `0x400f15`，而这段指令序列从函数的第 4 个字节开始，因此在 `0x400f18` 处存在一个 gadget，能够将 `%rax` 中的 64 位值复制到 `%rdi` 中。

**EN** Your code for RTARGET contains a number of functions similar to the `setval_210` function shown above in a region we refer to as the gadget farm. Your job will be to identify useful gadgets in the gadget farm and use these to perform attacks similar to those you did in Phases 2 and 3.

**中** 你的 RTARGET 在名为 **gadget farm** 的代码区域中包含多个类似 `setval_210` 的函数。你的任务是在该区域找到可用的 gadget，并用它们完成与第 2、3 阶段类似的目标。

**EN — Important:** The gadget farm is demarcated by functions `start_farm` and `end_farm` in your copy of `rtarget`. Do not attempt to construct gadgets from other portions of the program code.

**中 — 重要：** 在你的 `rtarget` 中，gadget farm 的边界由函数 `start_farm` 与 `end_farm` 确定。不得从程序的其他代码区域构造 gadget。

### 5.1 Level 2 — 第二关（Phase 4）

**EN** For Phase 4, you will repeat the attack of Phase 2, but do so on program RTARGET using gadgets from your gadget farm. You can construct your solution using gadgets consisting of the following instruction types, and using only the first eight x86-64 registers (`%rax`–`%rdi`).

**中** 在第 4 阶段，你需要在 RTARGET 上使用 gadget farm 中的 gadget，重现第 2 阶段的攻击目标。可以用下列指令类型构造 gadget，并且只使用 x86-64 的前 8 个寄存器（`%rax` 至 `%rdi`）。

- **EN** `movq`: The codes for these are shown in Figure 3A.  
  **中** `movq`：机器码参见原文图 3A。
- **EN** `popq`: The codes for these are shown in Figure 3B.  
  **中** `popq`：机器码参见原文图 3B。
- **EN** `ret`: This instruction is encoded by the single byte `0xc3`.  
  **中** `ret`：由单个字节 `0xc3` 编码。
- **EN** `nop`: This instruction (pronounced “no op,” which is short for “no operation”) is encoded by the single byte `0x90`. Its only effect is to cause the program counter to be incremented by 1.  
  **中** `nop`：读作 “no op”，是 “no operation（无操作）” 的缩写；机器码为单字节 `0x90`。它不改变通用寄存器中的数据，只让程序计数器前进 1 字节。

**EN — Some Advice:**

**中 — 一些建议：**

- **EN** All the gadgets you need can be found in the region of the code for `rtarget` demarcated by the functions `start_farm` and `mid_farm`.  
  **中** 本关需要的所有 gadget 都位于 `rtarget` 中 `start_farm` 与 `mid_farm` 之间的代码区域。
- **EN** You can do this attack with just two gadgets.  
  **中** 只用两个 gadget 就可以完成本关。
- **EN** When a gadget uses a `popq` instruction, it will pop data from the stack. As a result, your exploit string will contain a combination of gadget addresses and data.  
  **中** gadget 执行 `popq` 时会从栈上弹出数据。因此，攻击字符串需要同时包含 gadget 地址和数据。

### 5.2 Level 3 — 第三关（Phase 5）

**EN** Before you take on the Phase 5, pause to consider what you have accomplished so far. In Phases 2 and 3, you caused a program to execute machine code of your own design. If CTARGET had been a network server, you could have injected your own code into a distant machine. In Phase 4, you circumvented two of the main devices modern systems use to thwart buffer overflow attacks. Although you did not inject your own code, you were able inject a type of program that operates by stitching together sequences of existing code. You have also gotten 95/100 points for the lab. That’s a good score. If you have other pressing obligations consider stopping right now.

**中** 开始第 5 阶段前，可以回顾一下目前已经完成的工作。在第 2、3 阶段，你让程序执行了自己设计的机器码；如果 CTARGET 是一个网络服务器，就相当于能够向远程机器注入自己的代码。在第 4 阶段，你绕过了现代系统用于防止缓冲区溢出攻击的两类主要防护。虽然没有注入新代码，但你通过拼接已有代码片段，构造出了一种能够执行特定操作的“程序”。此时你已经获得 95/100 分，这是很好的成绩。如果还有其他紧迫任务，现在就停止实验也是可以的。

**EN** Phase 5 requires you to do an ROP attack on RTARGET to invoke function `touch3` with a pointer to a string representation of your cookie. That may not seem significantly more difficult than using an ROP attack to invoke `touch2`, except that we have made it so. Moreover, Phase 5 counts for only 5 points, which is not a true measure of the effort it will require. Think of it as more an extra credit problem for those who want to go beyond the normal expectations for the course.

**中** 第 5 阶段要求你对 RTARGET 实施 ROP 攻击，在调用 `touch3` 时传入一个指向 cookie 字符串表示的指针。乍看之下，这似乎并不比通过 ROP 调用 `touch2` 困难多少，但实验设计者刻意增加了难度。此外，第 5 阶段只占 5 分，远不能反映其实际工作量。可以将它视为希望超出课程基本要求的同学选择完成的附加挑战。

---

**译文范围说明：** 用户提供的文本截至第 10 页，未提供文中引用的附录 A、附录 B、图 3 以及参考文献的完整内容。图 2 的内容来自原文本提取结果，以上以概念文字示意替代原图，未声称是原图复刻。
