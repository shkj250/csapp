/* 
 * CS:APP Data Lab 
 * CS:APP 数据实验（位运算实验）
 * 
 * <Please put your name and userid here>
 * <请在此填写你的姓名和用户 ID>
 * 
 * bits.c - Source file with your solutions to the Lab.
 *          This is the file you will hand in to your instructor.
 * 中文：bits.c 是存放实验解答的源文件，也是需要提交给老师的文件。
 *
 * WARNING: Do not include the <stdio.h> header; it confuses the dlc
 * compiler. You can still use printf for debugging without including
 * <stdio.h>, although you might get a compiler warning. In general,
 * it's not good practice to ignore compiler warnings, but in this
 * case it's OK.  
 * 中文：不要包含 <stdio.h>，它会干扰 dlc 检查器。调试时可以直接使用
 * printf 而不包含该头文件，尽管编译器可能会警告。通常不应忽略编译警告，
 * 但这里这种情况可以接受。
 */

#if 0
/*
 * Instructions to Students:
 * 学生须知：
 *
 * STEP 1: Read the following instructions carefully.
 * 第一步：仔细阅读以下说明。
 */

You will provide your solution to the Data Lab by
editing the collection of functions in this source file.
中文：通过修改本源文件中的各个函数来完成 Data Lab。

INTEGER CODING RULES:
整数题编码规则：
 
  Replace the "return" statement in each function with one
  or more lines of C code that implements the function. Your code 
  must conform to the following style:
  中文：将各函数中的 return 语句替换为实现该函数的一行或多行 C 代码。
  代码必须遵循以下形式：
 
  int Funct(arg1, arg2, ...) {
      /* brief description of how your implementation works */
      /* 中文：简要说明你的实现原理。 */
      int var1 = Expr1;
      ...
      int varM = ExprM;

      varJ = ExprJ;
      ...
      varN = ExprN;
      return ExprR;
  }

  Each "Expr" is an expression using ONLY the following:
  中文：每个 Expr 表达式只能使用以下内容：
  1. Integer constants 0 through 255 (0xFF), inclusive. You are
      not allowed to use big constants such as 0xffffffff.
     中文：整数常量只能在 0 到 255（0xFF）之间，包含两端；
     不允许使用 0xffffffff 等较大的常量。
  2. Function arguments and local variables (no global variables).
     中文：函数参数和局部变量（不允许使用全局变量）。
  3. Unary integer operations ! ~
     中文：一元整数运算符 ! 和 ~。
  4. Binary integer operations & ^ | + << >>
     中文：二元整数运算符 &、^、|、+、<<、>>。
    
  Some of the problems restrict the set of allowed operators even further.
  Each "Expr" may consist of multiple operators. You are not restricted to
  one operator per line.
  中文：部分题目会进一步限制允许使用的运算符。每个 Expr 可以包含
  多个运算符，并不要求每行只能使用一个运算符。

  You are expressly forbidden to:
  中文：明确禁止以下行为：
  1. Use any control constructs such as if, do, while, for, switch, etc.
     中文：使用 if、do、while、for、switch 等任何控制结构。
  2. Define or use any macros.
     中文：定义或使用宏。
  3. Define any additional functions in this file.
     中文：在本文件中定义额外的函数。
  4. Call any functions.
     中文：调用任何函数。
  5. Use any other operations, such as &&, ||, -, or ?:
     中文：使用其他运算符，例如 &&、||、- 或 ?:。
  6. Use any form of casting.
     中文：使用任何形式的强制类型转换。
  7. Use any data type other than int.  This implies that you
     cannot use arrays, structs, or unions.
     中文：只能使用 int 类型，不能使用数组、结构体或联合体。

 
  You may assume that your machine:
  中文：可以假设机器满足以下条件：
  1. Uses 2s complement, 32-bit representations of integers.
     中文：整数采用 32 位补码表示。
  2. Performs right shifts arithmetically.
     中文：右移为算术右移，即高位用符号位填充。
  3. Has unpredictable behavior when shifting if the shift amount
     is less than 0 or greater than 31.
     中文：移位位数小于 0 或大于 31 时，行为不可预测。


EXAMPLES OF ACCEPTABLE CODING STYLE:
符合要求的代码风格示例：
  /*
   * pow2plus1 - returns 2^x + 1, where 0 <= x <= 31
   * 中文：pow2plus1 返回 2 的 x 次方加 1，其中 0 <= x <= 31。
   */
  int pow2plus1(int x) {
     /* exploit ability of shifts to compute powers of 2 */
     /* 中文：利用移位计算 2 的幂。 */
     return (1 << x) + 1;
  }

  /*
   * pow2plus4 - returns 2^x + 4, where 0 <= x <= 31
   * 中文：pow2plus4 返回 2 的 x 次方加 4，其中 0 <= x <= 31。
   */
  int pow2plus4(int x) {
     /* exploit ability of shifts to compute powers of 2 */
     /* 中文：利用移位计算 2 的幂。 */
     int result = (1 << x);
     result += 4;
     return result;
  }

FLOATING POINT CODING RULES
浮点题编码规则

For the problems that require you to implement floating-point operations,
the coding rules are less strict.  You are allowed to use looping and
conditional control.  You are allowed to use both ints and unsigneds.
You can use arbitrary integer and unsigned constants. You can use any arithmetic,
logical, or comparison operations on int or unsigned data.
中文：浮点题的编码限制较宽松，允许循环和条件控制，允许使用 int 和
unsigned 类型以及任意整数常量，也允许对这些整数数据进行算术、逻辑
和比较运算。

You are expressly forbidden to:
中文：明确禁止以下行为：
  1. Define or use any macros.
     中文：定义或使用宏。
  2. Define any additional functions in this file.
     中文：在本文件中定义额外的函数。
  3. Call any functions.
     中文：调用任何函数。
  4. Use any form of casting.
     中文：使用任何形式的强制类型转换。
  5. Use any data type other than int or unsigned.  This means that you
     cannot use arrays, structs, or unions.
     中文：只能使用 int 或 unsigned 类型，不能使用数组、结构体或联合体。
  6. Use any floating point data types, operations, or constants.
     中文：使用浮点类型、浮点运算或浮点常量。


NOTES:
注意事项：
  1. Use the dlc (data lab checker) compiler (described in the handout) to 
     check the legality of your solutions.
     中文：使用实验说明中介绍的 dlc 检查器，检查解答是否符合编码规则。
  2. Each function has a maximum number of operations (integer, logical,
     or comparison) that you are allowed to use for your implementation
     of the function.  The max operator count is checked by dlc.
     Note that assignment ('=') is not counted; you may use as many of
     these as you want without penalty.
     中文：每题都有运算次数上限（包括整数、逻辑和比较运算），由 dlc 检查。
     赋值运算符 = 不计入次数，可以按需使用。
  3. Use the btest test harness to check your functions for correctness.
     中文：使用 btest 测试程序检查函数结果是否正确。
  4. Use the BDD checker to formally verify your functions
     中文：使用 BDD 检查器对函数进行形式化验证。
  5. The maximum number of ops for each function is given in the
     header comment for each function. If there are any inconsistencies 
     between the maximum ops in the writeup and in this file, consider
     this file the authoritative source.
     中文：每题的运算次数上限写在函数前的注释中；如果实验说明与
     本文件中的上限不一致，以本文件为准。

/*
 * STEP 2: Modify the following functions according the coding rules.
 * 第二步：按照编码规则修改下面的函数。
 * 
 *   IMPORTANT. TO AVOID GRADING SURPRISES:
 * 重要提示：为了避免评分结果出乎预期：
 *   1. Use the dlc compiler to check that your solutions conform
 *      to the coding rules.
 *      中文：使用 dlc 检查解答是否符合编码规则。
 *   2. Use the BDD checker to formally verify that your solutions produce 
 *      the correct answers.
 *      中文：使用 BDD 检查器形式化验证解答的正确性。
 */


#endif
//1
/* 
 * bitXor - x^y using only ~ and & 
 * 中文：只使用 ~ 和 & 实现 x 与 y 的异或。
 *   Example / 示例: bitXor(4, 5) = 1
 *   Legal ops / 允许的运算符: ~ &
 *   Max ops / 最大运算次数: 14
 *   Rating / 难度等级: 1
 */
int bitXor(int x, int y) {
  int a = ~x&y;
  int b = ~y&x;
  // 的摩根 a|b=~(~a&~b) a&b=~(~a|~b)
  return  ~( ~a & ~b); 
}
/* 
 * tmin - return minimum two's complement integer 
 * 中文：返回 32 位补码整数的最小值。
 *   Legal ops / 允许的运算符: ! ~ & ^ | + << >>
 *   Max ops / 最大运算次数: 4
 *   Rating / 难度等级: 1
 */
int tmin(void) {
  int x =1;
  //左移
  return x<<31;

}
//2
/*
 * isTmax - returns 1 if x is the maximum, two's complement number,
 *     and 0 otherwise 
 * 中文：如果 x 是最大的 32 位补码整数，返回 1；否则返回 0。
 *   Legal ops / 允许的运算符: ! ~ & ^ | +
 *   Max ops / 最大运算次数: 10
 *   Rating / 难度等级: 1
 */
int isTmax(int x) {
  //!(a^b) 是== 
  //!!是两次! 
  //!notzero=0 ,!zero=1
  //!!notzero=1,!!zero=0
  //Tmax 和-1都满足!( (x+1) ^ (~x) ) 
  return !( (x+1) ^ (~x) ) & !!(x+1);
}
/* 
 * allOddBits - return 1 if all odd-numbered bits in word set to 1
 *   where bits are numbered from 0 (least significant) to 31 (most significant)
 * 中文：如果所有奇数编号的位都是 1，返回 1；否则返回 0。
 * 位编号从 0（最低位）到 31（最高位）。
 *   Examples / 示例: allOddBits(0xFFFFFFFD) = 0, allOddBits(0xAAAAAAAA) = 1
 *   Legal ops / 允许的运算符: ! ~ & ^ | + << >>
 *   Max ops / 最大运算次数: 12
 *   Rating / 难度等级: 2
 */
int allOddBits(int x) {
  //  toget mask=0xAAAAAAAA;
  int mask=0xAA;
  mask=mask | (mask<<8);
  mask=mask | (mask<<16);


  return !( (x & mask) ^ mask);
}
/* 
 * negate - return -x 
 * 中文：返回 x 的相反数 -x。
 *   Example / 示例: negate(1) = -1.
 *   Legal ops / 允许的运算符: ! ~ & ^ | + << >>
 *   Max ops / 最大运算次数: 5
 *   Rating / 难度等级: 2
 */
int negate(int x) {
  return ~x + 1;
}
//3
/* 
 * isAsciiDigit - return 1 if 0x30 <= x <= 0x39 (ASCII codes for characters '0' to '9')
 * 中文：如果 0x30 <= x <= 0x39（字符 '0' 到 '9' 的 ASCII 编码），
 * 返回 1；否则返回 0。
 *   Example / 示例: isAsciiDigit(0x35) = 1.
 *            isAsciiDigit(0x3a) = 0.
 *            isAsciiDigit(0x05) = 0.
 *   Legal ops / 允许的运算符: ! ~ & ^ | + << >>
 *   Max ops / 最大运算次数: 15
 *   Rating / 难度等级: 3
 */
int isAsciiDigit(int x) {
  int max_num=0x3A;
  int min_num=0x30;
  //a=x-max_num<=-1 a最高位是1
  //b=x-min_num>=0 b最高位是0 ~b最高位是1
  int neg_max=~max_num+1;
  int neg_min=~min_num+1;
  int a=x+neg_max;
  int b=x+neg_min;
  b=~b;
  return  (( (a>>31) & 1 ) & ( ( b>>31 ) & 1) );



}

//int isAsciiDigit(int x) {
//int below = x + (~0x30 + 1);  /* x - 0x30 */
// int above = 0x39 + (~x + 1);  /* 0x39 - x */
//below and above 都是31位是0
//  return !((below | above) >> 31);}


/*
 * conditional - same as x ? y : z 
 * 中文：实现 x ? y : z：x 非零时返回 y，x 为零时返回 z。
 *   Example / 示例: conditional(2,4,5) = 4
 *   Legal ops / 允许的运算符: ! ~ & ^ | + << >>
 *   Max ops / 最大运算次数: 16
 *   Rating / 难度等级: 3
 */
int conditional(int x, int y, int z) {
  int a= !x; //x==0,a=1 x!=0 a=0
  int b=a<<31;
  int c=b>>31; //x==0,c=1111..11 x!=0 c=0
  return (z&c )| (~c &y); //x==0,return z; x!=0 return y;


}
/* 
 * isLessOrEqual - if x <= y  then return 1, else return 0 
 * 中文：如果 x <= y，返回 1；否则返回 0。
 *   Example / 示例: isLessOrEqual(4,5) = 1.
 *   Legal ops / 允许的运算符: ! ~ & ^ | + << >>
 *   Max ops / 最大运算次数: 24
 *   Rating / 难度等级: 3
 */
int isLessOrEqual(int x, int y) {
  //a=y-x ,a>=0 
  int sx=(x>>31)&1; //x 负数为1 
  int sy=(y>>31)&1;
  int diff=y+(~x+1);
  int different=sx^sy; //符号相反为1
  // different==0 同号 return ！（diff>>31）

  //different==1 sy==0,sx==1 return 1; 
  //different==1 sy==1,sx==0,return 0; return different & sx

  return (different&sx) | ( !different & !(diff>>31) ) ;

}
//4
/* 
 * logicalNeg - implement the ! operator, using all of 
 *              the legal operators except !
 * 中文：使用除 ! 以外的合法运算符实现逻辑非运算 !。
 *   Examples / 示例: logicalNeg(3) = 0, logicalNeg(0) = 1
 *   Legal ops / 允许的运算符: ~ & ^ | + << >>
 *   Max ops / 最大运算次数: 12
 *   Rating / 难度等级: 4 
 */
int logicalNeg(int x) {
  return  ( ( x |(~x+1) )>>31 ) + 1;
  
}
/* howManyBits - return the minimum number of bits required to represent x in
 *             two's complement
 * 中文：返回用补码表示 x 所需的最少位数，包含符号位。
 *  Examples / 示例: howManyBits(12) = 5
 *            howManyBits(298) = 10
 *            howManyBits(-5) = 4
 *            howManyBits(0)  = 1
 *            howManyBits(-1) = 1
 *            howManyBits(0x80000000) = 32
 *  Legal ops / 允许的运算符: ! ~ & ^ | + << >>
 *  Max ops / 最大运算次数: 90
 *  Rating / 难度等级: 4
 */
int howManyBits(int x) {
//非负数：sign 全 0，x 不变。
//负数：sign 全 1，x 被取反。
//负数原来要找的“第一个 0”，取反后就变成了“第一个 1”。最后统一加一个符号位即可。
    int sign = x >> 31;
    int b16, b8, b4, b2, b1, b0;
    x = x ^ sign;

    b16 = (!!(x >> 16)) << 4;
    x = x >> b16;

    b8 = (!!(x >> 8)) << 3;
    x = x >> b8;

    b4 = (!!(x >> 4)) << 2;
    x = x >> b4;

    b2 = (!!(x >> 2)) << 1;
    x = x >> b2;

    b1 = !!(x >> 1);
    x = x >> b1;

    b0 = x;

    return b16 + b8 + b4 + b2 + b1 + b0 + 1;
}
//float / 浮点题
/* 
 * floatScale2 - Return bit-level equivalent of expression 2*f for
 *   floating point argument f.
 * 中文：对于浮点数 f，返回 2*f 的二进制位表示。
 *   Both the argument and result are passed as unsigned int's, but
 *   they are to be interpreted as the bit-level representation of
 *   single-precision floating point values.
 * 中文：参数和返回值使用 unsigned int 传递，但应将它们解释为
 * 单精度浮点数的二进制位表示。
 *   When argument is NaN, return argument
 * 中文：如果参数表示 NaN（非数），原样返回参数。
 *   Legal ops / 允许的运算符: Any integer/unsigned operations incl. ||, &&. also if, while
 *   中文：任意整数或无符号整数运算（包括 ||、&&），以及 if、while。
 *   Max ops / 最大运算次数: 30
 *   Rating / 难度等级: 4
 */
unsigned floatScale2(unsigned uf) {
  unsigned sign = uf & 0x80000000u;
  unsigned exp = (uf>>23) & 0xFF;
  unsigned frac = uf & 0x7FFFFF;
  if ( !(exp^ 0xFF)  || (!exp & !frac)){
   return uf;

  }
  else if (!exp){
   frac=frac<<1;
   return sign | frac;

  }
  else{
   exp=exp+1;
   if (exp==255){
      frac=0;
   }
   return sign | (exp<<23) | frac;
  }

}
/* 
 * floatFloat2Int - Return bit-level equivalent of expression (int) f
 *   for floating point argument f.
 * 中文：对于浮点数 f，返回其转换为 int 后的结果，向零截断。
 *   Argument is passed as unsigned int, but
 *   it is to be interpreted as the bit-level representation of a
 *   single-precision floating point value.
 * 中文：参数使用 unsigned int 传递，应将其解释为单精度浮点数的位表示。
 *   Anything out of range (including NaN and infinity) should return
 *   0x80000000u.
 * 中文：如果结果超出 int 范围（包括 NaN 和无穷大），返回 0x80000000u。
 *   Legal ops / 允许的运算符: Any integer/unsigned operations incl. ||, &&. also if, while
 *   中文：任意整数或无符号整数运算（包括 ||、&&），以及 if、while。
 *   Max ops / 最大运算次数: 30
 *   Rating / 难度等级: 4
 */
int floatFloat2Int(unsigned uf) {
  unsigned sign = uf>>31 &1;
  unsigned exp = (uf >>23)&0xFF;
  int E= exp-127;
  int result;
  unsigned new_frac;
  unsigned frac = uf & 0x7FFFFF;
  if (exp==255){
   return 0x80000000u;
  }

  if (E<0){
   return 0;
  }

  if (E>=31){
   return 0x80000000u;
  }

 new_frac=frac | 1<<23;

  if (E<23){
   result = new_frac>>(23-E);
  }
  else{
   result = new_frac<<(E-23);
  }

  if (sign==0){
   return result;
  }
  else{
   return ~result+1;
  }


}
/* 
 * floatPower2 - Return bit-level equivalent of the expression 2.0^x
 *   (2.0 raised to the power x) for any 32-bit integer x.
 * 中文：对于任意 32 位整数 x，返回 2 的 x 次方的单精度浮点位表示。
 *
 *   The unsigned value that is returned should have the identical bit
 *   representation as the single-precision floating-point number 2.0^x.
 * 中文：返回的 unsigned 值应与单精度浮点数 2 的 x 次方具有相同的位表示。
 *   If the result is too small to be represented as a denorm, return
 *   0. If too large, return +INF.
 * 中文：如果结果太小，连非规格化数也无法表示，返回 0；
 * 如果结果太大，返回正无穷 +INF 的位表示。
 * 
 *   Legal ops / 允许的运算符: Any integer/unsigned operations incl. ||, &&. Also if, while
 *   中文：任意整数或无符号整数运算（包括 ||、&&），以及 if、while。 
 *   Max ops / 最大运算次数: 30 
 *   Rating / 难度等级: 4
 */
unsigned floatPower2(int x) {
   //num=2**x
   //规格化的数frac=0, num=2**(exp-127) exp [1,254] x=[-126,127]
   //非规格化的数 num=frac*2**(-149) frac=[2**0,2**22] x=[-149,-127]
   unsigned posi_inf=0xFF<<23;
   //大于规格化数的上限
   if (x>127){
      return posi_inf;
    }
    //小于非规格化数的最小值
   if (x<-149){
      return 0;
    }
    //规格化数
   if(x>=-126){
      unsigned exp=x+127;
      return exp<<23;
    }
    //非规格化数
   return 1<<(x+149);
   
}
