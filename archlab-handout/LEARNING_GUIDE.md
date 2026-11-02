# Architecture Lab · 从指令到处理器

[总路线](../README.md) · 原始依据：[archlab.pdf](archlab.pdf)、[simguide.pdf](simguide.pdf)、[README](README)、`sim.tar`。

本实验是 Y86-64，不是直接编写 x86-64 程序。Part A 学汇编，Part B 扩展 SEQ，Part C 优化 PIPE 与 `ncopy`。全部原要求保留；可先把 Part A 做完，再分阶段进入硬件控制。

## 环境起步

当前工具还在压缩包中。仅在尚未解压、没有已有 `sim/` 工作目录时执行：

```sh
tar xf sim.tar
cd sim
make GUIMODE= TKLIBS= TKINC=
```

以上覆盖 Makefile 的 GUI/Tcl 设置，构建终端版，不修改提交文件。若已有 `sim/`，直接进入它继续，不覆盖自己的工作。旧工具遇到编译问题时先记录具体错误；工具可能需要 flex/bison，GUI 另需 Tcl/Tk。

外层 Makefile 的 `handin-*` 指向历史课程服务器，不是本地练习入口。特别注意 `sim/seq/Makefile` 的 `clean` 会删除该目录的 `*.ys`；不要把个人汇编练习放在那里，也不要不看规则就清理。

## 热身与关键知识

先读 `sim/y86-code/asum.ys`：标出初始栈、参数、循环和 `halt`。先手算，再执行：

```sh
cd misc
./yis ../y86-code/asum.yo
```

过关标准：理解 `.ys` 是汇编源文件，`.yo` 是 YAS 生成的目标文本；YIS 按 ISA（指令集架构）语义执行，SSIM/PSIM 按处理器模型执行。

| 概念 / Concept | 要回答的问题 |
|---|---|
| 指令语义 / Instruction semantics | 读哪些寄存器，写哪些寄存器，改哪些状态？ |
| HCL / Hardware Control Language | 当前输入下，控制信号选择哪条数据通路？ |
| 流水线冒险 / Pipeline hazard | 哪个阶段需要一个尚未就绪的值？ |
| 转发 / Forwarding | 值能否从前面的结果直接送到消费者？ |
| 停顿与气泡 / Stall and bubble | 哪个流水寄存器保持，哪个插入空操作？ |

## 阶段 1：Part A，先循环再递归

在 `sim/misc` 依次完成讲义要求的 `sum.ys`、`copy.ys`、`rsum.ys`。从单节点、空链表、小数组开始，再用讲义测试数据验证。`copy` 还需要返回规定的校验值，不能只检查目标数组。

在 `sim/misc` 执行（文件由你编写后）：

```sh
./yas sum.ys
./yis sum.yo
./yas copy.ys
./yis copy.yo
./yas rsum.ys
./yis rsum.yo
```

过关标准：结果符合讲义，内存写入正确，递归的调用与返回后栈平衡。你能画出一次递归调用中哪些数据必须跨调用保存。

<details>
<summary>提示 1：递归结果对不上</summary>

先检查终止条件，再检查跨调用仍需要的值是否被覆盖。跟踪一次 `call` 和 `ret` 的栈变化，把保存与恢复操作成对标记。

</details>

## 阶段 2：Part B，扩展 iaddq

在改 `seq-full.hcl` 前，为 `iaddq` 写阶段表：取指需要什么字段，译码读什么，ALU 输入是什么，是否更新条件码，写回给谁，PC 前进多少。

在 `sim/seq` 构建；以下所有 make 命令都可附带终端版覆盖参数：

```sh
make VERSION=full GUIMODE= TKLIBS= TKINC=
./ssim -t ../y86-code/asumi.yo
```

再到 `sim/y86-code` 跑 `make testssim`，到 `sim/ptest` 跑：

```sh
make SIM=../seq/ssim
make SIM=../seq/ssim TFLAGS=-i
```

过关标准：新指令正确，原指令回归也正确；不是只跑一个 `iaddq` 例子。保存你对每个控制信号修改理由的说明。

<details>
<summary>提示 2：新指令执行了，却有错误状态</summary>

“识别为合法指令”只是第一步。逐阶段核对字段长度、源寄存器、立即数、ALU 选择、目的寄存器和条件码更新。找第一个与 ISA 语义不一致的状态。

</details>

## 阶段 3：Part C，先正确性再 CPE

在 `sim/pipe` 修改前记录原 `ncopy.ys` 的基线。先分析一次迭代的依赖，再尝试调度、循环展开或控制策略；逐次只验证一个假设。不得把长度写死成测试规模。

```sh
make VERSION=full GUIMODE= TKLIBS= TKINC=
./correctness.pl
./correctness.pl -p
./benchmark.pl
```

YIS 正确与 PIPE 正确都要验证。再在 `sim/y86-code` 跑 `make testpsim`，在 `sim/ptest` 跑 `make SIM=../pipe/psim`；实现了 `iaddq` 时再跑带 `TFLAGS=-i` 的测试。

长度约束按讲义执行：YAS 生成 `ncopy.yo` 后，在 `sim/pipe` 用 `./check-len.pl < ncopy.yo` 检查。基准脚本不是正确性测试的替代品。

<details>
<summary>提示 3：展开后长数组快，小数组错</summary>

把长度拆成“完整批次＋剩余元素”，单独跟踪不足一批的情况。若 ISA 正确但 PIPE 错误，检查处理器控制、依赖和条件码，而非只盯着循环算法。

</details>

## 完整验收与进阶

Part A 三份程序满足讲义；Part B 新旧指令都通过回归；Part C 两种模拟器正确、长度合法、原指令集回归通过，并按讲义评价平均 CPE。指南分阶段不降低原性能目标。

进阶：画出一段指令序列的流水线时间表，逐周期解释停顿；预测一次调度如何减少周期，测量后说明差异。处理器与程序共同优化时，记录两边的改动，避免误把收益归给单一因素。
