This file contains materials for one instance of the attacklab.

中文：本文件说明 Attack Lab 某个具体实验实例的材料。

Files:

中文：文件说明：

    ctarget

中文：ctarget：下面介绍的代码注入目标。

Linux binary with code-injection vulnerability.  To be used for phases
1-3 of the assignment.

中文：带代码注入漏洞的 Linux 二进制程序，用于实验第 1–3 阶段。

    rtarget

中文：rtarget：下面介绍的返回导向编程目标。

Linux binary with return-oriented programming vulnerability.  To be
used for phases 4-5 of the assignment.

中文：带返回导向编程（ROP）漏洞的 Linux 二进制程序，用于第 4–5 阶段。

     cookie.txt

中文：cookie.txt：下面介绍的签名文件。

Text file containing 4-byte signature required for this lab instance.

中文：文本文件，包含本实验实例所需的 4 字节签名。

     farm.c

中文：farm.c：下面介绍的指令片段区域源码。

Source code for gadget farm present in this instance of rtarget.  You
can compile (use flag -Og) and disassemble it to look for gadgets.

中文：本实例 rtarget 的 gadget farm 源代码。可以使用 -Og 编译并反汇编，寻找可用的指
令片段（gadget）。

     hex2raw

中文：hex2raw：下面介绍的字节转换工具。

Utility program to generate byte sequences.  See documentation in lab
handout.

中文：生成字节序列的工具程序，使用方法见实验讲义。

