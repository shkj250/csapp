#/* $begin pipe-all-hcl */
####################################################################
#    HCL Description of Control for Pipelined Y86-64 Processor     #
# 中文：流水线 Y86-64 处理器的控制逻辑 HCL 描述
#    Copyright (C) Randal E. Bryant, David R. O'Hallaron, 2014     #
# 中文：版权所有，Randal E. Bryant 与 David R. O'Hallaron，2014
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
quote '#include "pipeline.h"'
quote '#include "stages.h"'
quote '#include "sim.h"'
quote 'int sim_main(int argc, char *argv[]);'
quote 'int main(int argc, char *argv[]){return sim_main(argc,argv);}'

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

##### Symbolic represenations of Y86-64 function codes            #####
# 中文：Y86-64 功能代码的符号表示
wordsig FNONE    'F_NONE'        # Default function code
# 中文：默认功能代码

##### Symbolic representation of Y86-64 Registers referenced      #####
# 中文：被引用的 Y86-64 寄存器的符号表示
wordsig RRSP     'REG_RSP'    	     # Stack Pointer
# 中文：栈指针
wordsig RNONE    'REG_NONE'   	     # Special value indicating "no register"
# 中文：表示“不使用寄存器”的特殊值

##### ALU Functions referenced explicitly ##########################
# 中文：显式引用的 ALU 运算
wordsig ALUADD	'A_ADD'		     # ALU should add its arguments
# 中文：ALU 应将两个输入相加

##### Possible instruction status values                       #####
# 中文：指令可能的状态值
wordsig SBUB	'STAT_BUB'	# Bubble in stage
# 中文：当前阶段为气泡
wordsig SAOK	'STAT_AOK'	# Normal execution
# 中文：正常执行
wordsig SADR	'STAT_ADR'	# Invalid memory address
# 中文：无效内存地址
wordsig SINS	'STAT_INS'	# Invalid instruction
# 中文：无效指令
wordsig SHLT	'STAT_HLT'	# Halt instruction encountered
# 中文：遇到 halt 停机指令

##### Signals that can be referenced by control logic ##############
# 中文：控制逻辑可以引用的信号

##### Pipeline Register F ##########################################
# 中文：流水寄存器 F

wordsig F_predPC 'pc_curr->pc'	     # Predicted value of PC
# 中文：PC 的预测值

##### Intermediate Values in Fetch Stage ###########################
# 中文：取指阶段的中间值

wordsig imem_icode  'imem_icode'      # icode field from instruction memory
# 中文：来自指令内存的 icode 字段
wordsig imem_ifun   'imem_ifun'       # ifun  field from instruction memory
# 中文：来自指令内存的 ifun 字段
wordsig f_icode	'if_id_next->icode'  # (Possibly modified) instruction code
# 中文：指令代码，可能经过修改
wordsig f_ifun	'if_id_next->ifun'   # Fetched instruction function
# 中文：取出的指令功能代码
wordsig f_valC	'if_id_next->valc'   # Constant data of fetched instruction
# 中文：取出的指令中的常数
wordsig f_valP	'if_id_next->valp'   # Address of following instruction
# 中文：下一条指令的地址
boolsig imem_error 'imem_error'	     # Error signal from instruction memory
# 中文：指令内存的错误信号
boolsig instr_valid 'instr_valid'    # Is fetched instruction valid?
# 中文：取出的指令是否合法？

##### Pipeline Register D ##########################################
# 中文：流水寄存器 D
wordsig D_icode 'if_id_curr->icode'   # Instruction code
# 中文：指令代码
wordsig D_rA 'if_id_curr->ra'	     # rA field from instruction
# 中文：指令中的 rA 字段
wordsig D_rB 'if_id_curr->rb'	     # rB field from instruction
# 中文：指令中的 rB 字段
wordsig D_valP 'if_id_curr->valp'     # Incremented PC
# 中文：递增后的 PC

##### Intermediate Values in Decode Stage  #########################
# 中文：译码阶段的中间值

wordsig d_srcA	 'id_ex_next->srca'  # srcA from decoded instruction
# 中文：译码得到的 srcA
wordsig d_srcB	 'id_ex_next->srcb'  # srcB from decoded instruction
# 中文：译码得到的 srcB
wordsig d_rvalA 'd_regvala'	     # valA read from register file
# 中文：从寄存器文件读出的 valA
wordsig d_rvalB 'd_regvalb'	     # valB read from register file
# 中文：从寄存器文件读出的 valB

##### Pipeline Register E ##########################################
# 中文：流水寄存器 E
wordsig E_icode 'id_ex_curr->icode'   # Instruction code
# 中文：指令代码
wordsig E_ifun  'id_ex_curr->ifun'    # Instruction function
# 中文：指令功能代码
wordsig E_valC  'id_ex_curr->valc'    # Constant data
# 中文：常数数据
wordsig E_srcA  'id_ex_curr->srca'    # Source A register ID
# 中文：源寄存器 A 的编号
wordsig E_valA  'id_ex_curr->vala'    # Source A value
# 中文：源操作数 A 的值
wordsig E_srcB  'id_ex_curr->srcb'    # Source B register ID
# 中文：源寄存器 B 的编号
wordsig E_valB  'id_ex_curr->valb'    # Source B value
# 中文：源操作数 B 的值
wordsig E_dstE 'id_ex_curr->deste'    # Destination E register ID
# 中文：E 结果的目的寄存器编号
wordsig E_dstM 'id_ex_curr->destm'    # Destination M register ID
# 中文：M 结果的目的寄存器编号

##### Intermediate Values in Execute Stage #########################
# 中文：执行阶段的中间值
wordsig e_valE 'ex_mem_next->vale'	# valE generated by ALU
# 中文：ALU 生成的 valE
boolsig e_Cnd 'ex_mem_next->takebranch' # Does condition hold?
# 中文：条件是否成立？
wordsig e_dstE 'ex_mem_next->deste'      # dstE (possibly modified to be RNONE)
# 中文：dstE，可能被改为 RNONE

##### Pipeline Register M                  #########################
# 中文：流水寄存器 M
wordsig M_stat 'ex_mem_curr->status'     # Instruction status
# 中文：指令状态
wordsig M_icode 'ex_mem_curr->icode'	# Instruction code
# 中文：指令代码
wordsig M_ifun  'ex_mem_curr->ifun'	# Instruction function
# 中文：指令功能代码
wordsig M_valA  'ex_mem_curr->vala'      # Source A value
# 中文：源操作数 A 的值
wordsig M_dstE 'ex_mem_curr->deste'	# Destination E register ID
# 中文：E 结果的目的寄存器编号
wordsig M_valE  'ex_mem_curr->vale'      # ALU E value
# 中文：ALU 的 E 结果值
wordsig M_dstM 'ex_mem_curr->destm'	# Destination M register ID
# 中文：M 结果的目的寄存器编号
boolsig M_Cnd 'ex_mem_curr->takebranch'	# Condition flag
# 中文：条件标志
boolsig dmem_error 'dmem_error'	        # Error signal from instruction memory
# 中文：指令内存的错误信号

##### Intermediate Values in Memory Stage ##########################
# 中文：访存阶段的中间值
wordsig m_valM 'mem_wb_next->valm'	# valM generated by memory
# 中文：内存生成的 valM
wordsig m_stat 'mem_wb_next->status'	# stat (possibly modified to be SADR)
# 中文：stat，可能被改为 SADR

##### Pipeline Register W ##########################################
# 中文：流水寄存器 W
wordsig W_stat 'mem_wb_curr->status'     # Instruction status
# 中文：指令状态
wordsig W_icode 'mem_wb_curr->icode'	# Instruction code
# 中文：指令代码
wordsig W_dstE 'mem_wb_curr->deste'	# Destination E register ID
# 中文：E 结果的目的寄存器编号
wordsig W_valE  'mem_wb_curr->vale'      # ALU E value
# 中文：ALU 的 E 结果值
wordsig W_dstM 'mem_wb_curr->destm'	# Destination M register ID
# 中文：M 结果的目的寄存器编号
wordsig W_valM  'mem_wb_curr->valm'	# Memory M value
# 中文：内存的 M 结果值

####################################################################
#    Control Signal Definitions.                                   #
# 中文：控制信号定义
####################################################################

################ Fetch Stage     ###################################
# 中文：取指阶段

## What address should instruction be fetched at
# 中文：应从哪个地址取指？
word f_pc = [
	# Mispredicted branch.  Fetch at incremented PC
	# 中文：分支预测错误：从递增后的 PC 取指
	M_icode == IJXX && !M_Cnd : M_valA;
	# Completion of RET instruction
	# 中文：ret 指令执行完成
	W_icode == IRET : W_valM;
	# Default: Use predicted value of PC
	# 中文：默认：使用预测的 PC
	1 : F_predPC;
];

## Determine icode of fetched instruction
# 中文：确定取出指令的 icode
word f_icode = [
	imem_error : INOP;
	1: imem_icode;
];

# Determine ifun
# 中文：确定 ifun
word f_ifun = [
	imem_error : FNONE;
	1: imem_ifun;
];

# Is instruction valid?
# 中文：指令是否合法？
bool instr_valid = f_icode in 
	{ INOP, IHALT, IRRMOVQ, IIRMOVQ, IRMMOVQ, IMRMOVQ,
	  IOPQ, IJXX, ICALL, IRET, IPUSHQ, IPOPQ };

# Determine status code for fetched instruction
# 中文：确定取出指令的状态码
word f_stat = [
	imem_error: SADR;
	!instr_valid : SINS;
	f_icode == IHALT : SHLT;
	1 : SAOK;
];

# Does fetched instruction require a regid byte?
# 中文：取出的指令是否需要寄存器编号字节？
bool need_regids =
	f_icode in { IRRMOVQ, IOPQ, IPUSHQ, IPOPQ, 
		     IIRMOVQ, IRMMOVQ, IMRMOVQ };

# Does fetched instruction require a constant word?
# 中文：取出的指令是否需要常数字？
bool need_valC =
	f_icode in { IIRMOVQ, IRMMOVQ, IMRMOVQ, IJXX, ICALL };

# Predict next value of PC
# 中文：预测下一个 PC
word f_predPC = [
	f_icode in { IJXX, ICALL } : f_valC;
	1 : f_valP;
];

################ Decode Stage ######################################
# 中文：译码阶段


## What register should be used as the A source?
# 中文：A 源操作数使用哪个寄存器？
word d_srcA = [
	D_icode in { IRRMOVQ, IRMMOVQ, IOPQ, IPUSHQ  } : D_rA;
	D_icode in { IPOPQ, IRET } : RRSP;
	1 : RNONE; # Don't need register
	# 中文：不需要寄存器
];

## What register should be used as the B source?
# 中文：B 源操作数使用哪个寄存器？
word d_srcB = [
	D_icode in { IOPQ, IRMMOVQ, IMRMOVQ  } : D_rB;
	D_icode in { IPUSHQ, IPOPQ, ICALL, IRET } : RRSP;
	1 : RNONE;  # Don't need register
	# 中文：不需要寄存器
];

## What register should be used as the E destination?
# 中文：E 结果写入哪个目的寄存器？
word d_dstE = [
	D_icode in { IRRMOVQ, IIRMOVQ, IOPQ} : D_rB;
	D_icode in { IPUSHQ, IPOPQ, ICALL, IRET } : RRSP;
	1 : RNONE;  # Don't write any register
	# 中文：不写任何寄存器
];

## What register should be used as the M destination?
# 中文：M 结果写入哪个目的寄存器？
word d_dstM = [
	D_icode in { IMRMOVQ, IPOPQ } : D_rA;
	1 : RNONE;  # Don't write any register
	# 中文：不写任何寄存器
];

## What should be the A value?
# 中文：A 操作数应取什么值？
## Forward into decode stage for valA
# 中文：把 valA 转发到译码阶段
word d_valA = [
	D_icode in { ICALL, IJXX } : D_valP; # Use incremented PC
	# 中文：使用递增后的 PC
	d_srcA == e_dstE : e_valE;    # Forward valE from execute
	# 中文：从执行阶段转发 valE
	d_srcA == M_dstM : m_valM;    # Forward valM from memory
	# 中文：从访存阶段转发 valM
	d_srcA == M_dstE : M_valE;    # Forward valE from memory
	# 中文：从访存阶段转发 valE
	d_srcA == W_dstM : W_valM;    # Forward valM from write back
	# 中文：从写回阶段转发 valM
	d_srcA == W_dstE : W_valE;    # Forward valE from write back
	# 中文：从写回阶段转发 valE
	1 : d_rvalA;  # Use value read from register file
	# 中文：使用寄存器文件读出的值
];

word d_valB = [
	d_srcB == e_dstE : e_valE;    # Forward valE from execute
	# 中文：从执行阶段转发 valE
	d_srcB == M_dstM : m_valM;    # Forward valM from memory
	# 中文：从访存阶段转发 valM
	d_srcB == M_dstE : M_valE;    # Forward valE from memory
	# 中文：从访存阶段转发 valE
	d_srcB == W_dstM : W_valM;    # Forward valM from write back
	# 中文：从写回阶段转发 valM
	d_srcB == W_dstE : W_valE;    # Forward valE from write back
	# 中文：从写回阶段转发 valE
	1 : d_rvalB;  # Use value read from register file
	# 中文：使用寄存器文件读出的值
];

################ Execute Stage #####################################
# 中文：执行阶段

## Select input A to ALU
# 中文：选择 ALU 的 A 输入
word aluA = [
	E_icode in { IRRMOVQ, IOPQ } : E_valA;
	E_icode in { IIRMOVQ, IRMMOVQ, IMRMOVQ } : E_valC;
	E_icode in { ICALL, IPUSHQ } : -8;
	E_icode in { IRET, IPOPQ } : 8;
	# Other instructions don't need ALU
	# 中文：其他指令不需要 ALU
];

## Select input B to ALU
# 中文：选择 ALU 的 B 输入
word aluB = [
	E_icode in { IRMMOVQ, IMRMOVQ, IOPQ, ICALL, 
		     IPUSHQ, IRET, IPOPQ } : E_valB;
	E_icode in { IRRMOVQ, IIRMOVQ } : 0;
	# Other instructions don't need ALU
	# 中文：其他指令不需要 ALU
];

## Set the ALU function
# 中文：设置 ALU 运算功能
word alufun = [
	E_icode == IOPQ : E_ifun;
	1 : ALUADD;
];

## Should the condition codes be updated?
# 中文：是否更新条件码？
bool set_cc = E_icode == IOPQ &&
	# State changes only during normal operation
	# 中文：只在正常执行状态下修改状态
	!m_stat in { SADR, SINS, SHLT } && !W_stat in { SADR, SINS, SHLT };

## Generate valA in execute stage
# 中文：在执行阶段生成 valA
word e_valA = E_valA;    # Pass valA through stage
# 中文：将 valA 原样传过该阶段

## Set dstE to RNONE in event of not-taken conditional move
# 中文：条件传送不成立时，将 dstE 设为 RNONE
word e_dstE = [
	E_icode == IRRMOVQ && !e_Cnd : RNONE;
	1 : E_dstE;
];

################ Memory Stage ######################################
# 中文：访存阶段

## Select memory address
# 中文：选择内存访问地址
word mem_addr = [
	M_icode in { IRMMOVQ, IPUSHQ, ICALL, IMRMOVQ } : M_valE;
	M_icode in { IPOPQ, IRET } : M_valA;
	# Other instructions don't need address
	# 中文：其他指令不需要访存地址
];

## Set read control signal
# 中文：设置读控制信号
bool mem_read = M_icode in { IMRMOVQ, IPOPQ, IRET };

## Set write control signal
# 中文：设置写控制信号
bool mem_write = M_icode in { IRMMOVQ, IPUSHQ, ICALL };

#/* $begin pipe-m_stat-hcl */
## Update the status
# 中文：更新状态
word m_stat = [
	dmem_error : SADR;
	1 : M_stat;
];
#/* $end pipe-m_stat-hcl */

## Set E port register ID
# 中文：设置 E 写回端口的寄存器编号
word w_dstE = W_dstE;

## Set E port value
# 中文：设置 E 写回端口的值
word w_valE = W_valE;

## Set M port register ID
# 中文：设置 M 写回端口的寄存器编号
word w_dstM = W_dstM;

## Set M port value
# 中文：设置 M 写回端口的值
word w_valM = W_valM;

## Update processor status
# 中文：更新处理器状态
word Stat = [
	W_stat == SBUB : SAOK;
	1 : W_stat;
];

################ Pipeline Register Control #########################
# 中文：流水寄存器控制

# Should I stall or inject a bubble into Pipeline Register F?
# 中文：是否让流水寄存器 F 停顿或注入气泡？
# At most one of these can be true.
# 中文：两者最多有一个为真
bool F_bubble = 0;
bool F_stall =
	# Conditions for a load/use hazard
	# 中文：加载/使用冒险的条件
	E_icode in { IMRMOVQ, IPOPQ } &&
	 E_dstM in { d_srcA, d_srcB } ||
	# Stalling at fetch while ret passes through pipeline
	# 中文：ret 通过流水线期间，在取指阶段停顿
	IRET in { D_icode, E_icode, M_icode };

# Should I stall or inject a bubble into Pipeline Register D?
# 中文：是否让流水寄存器 D 停顿或注入气泡？
# At most one of these can be true.
# 中文：两者最多有一个为真
bool D_stall = 
	# Conditions for a load/use hazard
	# 中文：加载/使用冒险的条件
	E_icode in { IMRMOVQ, IPOPQ } &&
	 E_dstM in { d_srcA, d_srcB };

bool D_bubble =
	# Mispredicted branch
	# 中文：分支预测错误
	(E_icode == IJXX && !e_Cnd) ||
	# Stalling at fetch while ret passes through pipeline
	# 中文：ret 通过流水线期间，在取指阶段停顿
	# but not condition for a load/use hazard
	# 中文：但不是加载/使用冒险的情形
	!(E_icode in { IMRMOVQ, IPOPQ } && E_dstM in { d_srcA, d_srcB }) &&
	  IRET in { D_icode, E_icode, M_icode };

# Should I stall or inject a bubble into Pipeline Register E?
# 中文：是否让流水寄存器 E 停顿或注入气泡？
# At most one of these can be true.
# 中文：两者最多有一个为真
bool E_stall = 0;
bool E_bubble =
	# Mispredicted branch
	# 中文：分支预测错误
	(E_icode == IJXX && !e_Cnd) ||
	# Conditions for a load/use hazard
	# 中文：加载/使用冒险的条件
	E_icode in { IMRMOVQ, IPOPQ } &&
	 E_dstM in { d_srcA, d_srcB};

# Should I stall or inject a bubble into Pipeline Register M?
# 中文：是否让流水寄存器 M 停顿或注入气泡？
# At most one of these can be true.
# 中文：两者最多有一个为真
bool M_stall = 0;
# Start injecting bubbles as soon as exception passes through memory stage
# 中文：异常通过访存阶段时，立即开始注入气泡
bool M_bubble = m_stat in { SADR, SINS, SHLT } || W_stat in { SADR, SINS, SHLT };

# Should I stall or inject a bubble into Pipeline Register W?
# 中文：是否让流水寄存器 W 停顿或注入气泡？
bool W_stall = W_stat in { SADR, SINS, SHLT };
bool W_bubble = 0;
#/* $end pipe-all-hcl */
