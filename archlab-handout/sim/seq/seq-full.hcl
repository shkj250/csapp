#/* $begin seq-all-hcl */
####################################################################
#  HCL Description of Control for Single Cycle Y86-64 Processor SEQ   #
# 中文：单周期 Y86-64 处理器 SEQ 的控制逻辑 HCL 描述
#  Copyright (C) Randal E. Bryant, David R. O'Hallaron, 2010       #
# 中文：版权所有，Randal E. Bryant 与 David R. O'Hallaron，2010
####################################################################

## Your task is to implement the iaddq instruction
# 中文：你的任务是实现 iaddq 指令
## The file contains a declaration of the icodes
# 中文：本文件包含指令代码的声明
## for iaddq (IIADDQ)
# 中文：其中 iaddq 的指令代码符号为 IIADDQ
## Your job is to add the rest of the logic to make it work
# 中文：你需要补齐其余逻辑，使该指令正确工作

####################################################################
#    C Include's.  Don't alter these                               #
# 中文：C 头文件与嵌入代码；不要修改
####################################################################

quote '#include <stdio.h>'
quote '#include "isa.h"'
quote '#include "sim.h"'
quote 'int sim_main(int argc, char *argv[]);'
quote 'word_t gen_pc(){return 0;}'
quote 'int main(int argc, char *argv[])'
quote '  {plusmode=0;return sim_main(argc,argv);}'

####################################################################
#    Declarations.  Do not change/remove/delete any of these       #
# 中文：声明部分；不得修改、移除或删除以下任何声明
####################################################################

##### Symbolic representation of Y86-64 Instruction Codes #############
# 中文：Y86-64 指令代码的符号表示
wordsig INOP 	'I_NOP'
wordsig IHALT	'I_HALT'
wordsig IRRMOVQ	'I_RRMOVQ'
wordsig IIRMOVQ	'I_IRMOVQ'
wordsig IRMMOVQ	'I_RMMOVQ'
wordsig IMRMOVQ	'I_MRMOVQ'
wordsig IOPQ	'I_ALU'
wordsig IJXX	'I_JMP'
wordsig ICALL	'I_CALL'
wordsig IRET	'I_RET'
wordsig IPUSHQ	'I_PUSHQ'
wordsig IPOPQ	'I_POPQ'
# Instruction code for iaddq instruction
# 中文：iaddq 指令的代码
wordsig IIADDQ	'I_IADDQ'

##### Symbolic represenations of Y86-64 function codes                  #####
# 中文：Y86-64 功能代码的符号表示
wordsig FNONE    'F_NONE'        # Default function code
# 中文：默认功能代码

##### Symbolic representation of Y86-64 Registers referenced explicitly #####
# 中文：显式引用的 Y86-64 寄存器的符号表示
wordsig RRSP     'REG_RSP'    	# Stack Pointer
# 中文：栈指针
wordsig RNONE    'REG_NONE'   	# Special value indicating "no register"
# 中文：表示“不使用寄存器”的特殊值

##### ALU Functions referenced explicitly                            #####
# 中文：显式引用的 ALU 运算
wordsig ALUADD	'A_ADD'		# ALU should add its arguments
# 中文：ALU 应将两个输入相加

##### Possible instruction status values                             #####
# 中文：指令可能的状态值
wordsig SAOK	'STAT_AOK'	# Normal execution
# 中文：正常执行
wordsig SADR	'STAT_ADR'	# Invalid memory address
# 中文：无效内存地址
wordsig SINS	'STAT_INS'	# Invalid instruction
# 中文：无效指令
wordsig SHLT	'STAT_HLT'	# Halt instruction encountered
# 中文：遇到 halt 停机指令

##### Signals that can be referenced by control logic ####################
# 中文：控制逻辑可以引用的信号

##### Fetch stage inputs		#####
# 中文：取指阶段的输入
wordsig pc 'pc'				# Program counter
# 中文：程序计数器（PC）
##### Fetch stage computations		#####
# 中文：取指阶段的计算
wordsig imem_icode 'imem_icode'		# icode field from instruction memory
# 中文：来自指令内存的 icode 字段
wordsig imem_ifun  'imem_ifun' 		# ifun field from instruction memory
# 中文：来自指令内存的 ifun 字段
wordsig icode	  'icode'		# Instruction control code
# 中文：指令控制代码
wordsig ifun	  'ifun'		# Instruction function
# 中文：指令功能代码
wordsig rA	  'ra'			# rA field from instruction
# 中文：指令中的 rA 字段
wordsig rB	  'rb'			# rB field from instruction
# 中文：指令中的 rB 字段
wordsig valC	  'valc'		# Constant from instruction
# 中文：指令中的常数
wordsig valP	  'valp'		# Address of following instruction
# 中文：下一条指令的地址
boolsig imem_error 'imem_error'		# Error signal from instruction memory
# 中文：指令内存的错误信号
boolsig instr_valid 'instr_valid'	# Is fetched instruction valid?
# 中文：取出的指令是否合法？

##### Decode stage computations		#####
# 中文：译码阶段的计算
wordsig valA	'vala'			# Value from register A port
# 中文：从寄存器文件 A 端口读出的值
wordsig valB	'valb'			# Value from register B port
# 中文：从寄存器文件 B 端口读出的值

##### Execute stage computations	#####
# 中文：执行阶段的计算
wordsig valE	'vale'			# Value computed by ALU
# 中文：ALU 计算出的值
boolsig Cnd	'cond'			# Branch test
# 中文：分支条件判定结果

##### Memory stage computations		#####
# 中文：访存阶段的计算
wordsig valM	'valm'			# Value read from memory
# 中文：从内存读取的值
boolsig dmem_error 'dmem_error'		# Error signal from data memory
# 中文：数据内存的错误信号


####################################################################
#    Control Signal Definitions.                                   #
# 中文：控制信号定义
####################################################################

################ Fetch Stage     ###################################
# 中文：取指阶段

# Determine instruction code
# 中文：确定指令代码
word icode = [
	imem_error: INOP;
	1: imem_icode;		# Default: get from instruction memory
	# 中文：默认：从指令内存取得
];

# Determine instruction function
# 中文：确定指令功能代码
word ifun = [
	imem_error: FNONE;
	1: imem_ifun;		# Default: get from instruction memory
	# 中文：默认：从指令内存取得
];

bool instr_valid = icode in 
	{ INOP, IHALT, IRRMOVQ, IIRMOVQ, IRMMOVQ, IMRMOVQ,
	       IOPQ, IJXX, ICALL, IRET, IPUSHQ, IPOPQ };

# Does fetched instruction require a regid byte?
# 中文：取出的指令是否需要寄存器编号字节？
bool need_regids =
	icode in { IRRMOVQ, IOPQ, IPUSHQ, IPOPQ, 
		     IIRMOVQ, IRMMOVQ, IMRMOVQ };

# Does fetched instruction require a constant word?
# 中文：取出的指令是否需要常数字？
bool need_valC =
	icode in { IIRMOVQ, IRMMOVQ, IMRMOVQ, IJXX, ICALL };

################ Decode Stage    ###################################
# 中文：译码阶段

## What register should be used as the A source?
# 中文：A 源操作数使用哪个寄存器？
word srcA = [
	icode in { IRRMOVQ, IRMMOVQ, IOPQ, IPUSHQ  } : rA;
	icode in { IPOPQ, IRET } : RRSP;
	1 : RNONE; # Don't need register
	# 中文：不需要寄存器
];

## What register should be used as the B source?
# 中文：B 源操作数使用哪个寄存器？
word srcB = [
	icode in { IOPQ, IRMMOVQ, IMRMOVQ  } : rB;
	icode in { IPUSHQ, IPOPQ, ICALL, IRET } : RRSP;
	1 : RNONE;  # Don't need register
	# 中文：不需要寄存器
];

## What register should be used as the E destination?
# 中文：E 结果写入哪个目的寄存器？
word dstE = [
	icode in { IRRMOVQ } && Cnd : rB;
	icode in { IIRMOVQ, IOPQ} : rB;
	icode in { IPUSHQ, IPOPQ, ICALL, IRET } : RRSP;
	1 : RNONE;  # Don't write any register
	# 中文：不写任何寄存器
];

## What register should be used as the M destination?
# 中文：M 结果写入哪个目的寄存器？
word dstM = [
	icode in { IMRMOVQ, IPOPQ } : rA;
	1 : RNONE;  # Don't write any register
	# 中文：不写任何寄存器
];

################ Execute Stage   ###################################
# 中文：执行阶段

## Select input A to ALU
# 中文：选择 ALU 的 A 输入
word aluA = [
	icode in { IRRMOVQ, IOPQ } : valA;
	icode in { IIRMOVQ, IRMMOVQ, IMRMOVQ } : valC;
	icode in { ICALL, IPUSHQ } : -8;
	icode in { IRET, IPOPQ } : 8;
	# Other instructions don't need ALU
	# 中文：其他指令不需要 ALU
];

## Select input B to ALU
# 中文：选择 ALU 的 B 输入
word aluB = [
	icode in { IRMMOVQ, IMRMOVQ, IOPQ, ICALL, 
		      IPUSHQ, IRET, IPOPQ } : valB;
	icode in { IRRMOVQ, IIRMOVQ } : 0;
	# Other instructions don't need ALU
	# 中文：其他指令不需要 ALU
];

## Set the ALU function
# 中文：设置 ALU 运算功能
word alufun = [
	icode == IOPQ : ifun;
	1 : ALUADD;
];

## Should the condition codes be updated?
# 中文：是否更新条件码？
bool set_cc = icode in { IOPQ };

################ Memory Stage    ###################################
# 中文：访存阶段

## Set read control signal
# 中文：设置读控制信号
bool mem_read = icode in { IMRMOVQ, IPOPQ, IRET };

## Set write control signal
# 中文：设置写控制信号
bool mem_write = icode in { IRMMOVQ, IPUSHQ, ICALL };

## Select memory address
# 中文：选择内存访问地址
word mem_addr = [
	icode in { IRMMOVQ, IPUSHQ, ICALL, IMRMOVQ } : valE;
	icode in { IPOPQ, IRET } : valA;
	# Other instructions don't need address
	# 中文：其他指令不需要访存地址
];

## Select memory input data
# 中文：选择写入内存的数据
word mem_data = [
	# Value from register
	# 中文：来自寄存器的值
	icode in { IRMMOVQ, IPUSHQ } : valA;
	# Return PC
	# 中文：返回地址 PC
	icode == ICALL : valP;
	# Default: Don't write anything
	# 中文：默认：不写入任何数据
];

## Determine instruction status
# 中文：确定指令状态
word Stat = [
	imem_error || dmem_error : SADR;
	!instr_valid: SINS;
	icode == IHALT : SHLT;
	1 : SAOK;
];

################ Program Counter Update ############################
# 中文：更新程序计数器

## What address should instruction be fetched at
# 中文：应从哪个地址取指？

word new_pc = [
	# Call.  Use instruction constant
	# 中文：调用：使用指令中的常数地址
	icode == ICALL : valC;
	# Taken branch.  Use instruction constant
	# 中文：分支成立：使用指令中的常数地址
	icode == IJXX && Cnd : valC;
	# Completion of RET instruction.  Use value from stack
	# 中文：ret 执行完成：使用从栈中取出的值
	icode == IRET : valM;
	# Default: Use incremented PC
	# 中文：默认：使用递增后的 PC
	1 : valP;
];
#/* $end seq-all-hcl */
