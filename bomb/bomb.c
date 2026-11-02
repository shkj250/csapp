/***************************************************************************
 * Dr. Evil's Insidious Bomb, Version 1.1
 * Copyright 2011, Dr. Evil Incorporated. All rights reserved.
 *
 * LICENSE:
 *
 * Dr. Evil Incorporated (the PERPETRATOR) hereby grants you (the
 * VICTIM) explicit permission to use this bomb (the BOMB).  This is a
 * time limited license, which expires on the death of the VICTIM.
 * The PERPETRATOR takes no responsibility for damage, frustration,
 * insanity, bug-eyes, carpal-tunnel syndrome, loss of sleep, or other
 * harm to the VICTIM.  Unless the PERPETRATOR wants to take credit,
 * that is.  The VICTIM may not distribute this bomb source code to
 * any enemies of the PERPETRATOR.  No VICTIM may debug,
 * reverse-engineer, run "strings" on, decompile, decrypt, or use any
 * other technique to gain knowledge of and defuse the BOMB.  BOMB
 * proof clothing may not be worn when handling this program.  The
 * PERPETRATOR will not apologize for the PERPETRATOR's poor sense of
 * humor.  This license is null and void where the BOMB is prohibited
 * by law.
 * 中文：邪恶博士的阴险炸弹，版本 1.1。版权所有 2011，邪恶博士公司，保留所有权利。许可：
 * 肇事者（邪恶博士公司）明确允许受害者（你）使用本炸弹。本许可有时间限制，在受害者死亡
 * 时到期。肇事者不对伤害、挫折、精神失常、眼睛凸出、腕管综合征、失眠或其他伤害负责，除
 * 非肇事者想把这些当作自己的功劳。受害者不得向肇事者的敌人分发炸弹源码；不得调试、逆向、
 * 运行 strings、反编译、解密或以其他方式了解并拆除炸弹；操作程序时不得穿防爆服。肇事者
 * 不会为自己糟糕的幽默感道歉。在法律禁止炸弹的地方，本许可无效。（以上是原文的戏谑叙述；
 * 实验实际学习要求以 bomblab.pdf 为准。）
 ***************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include "support.h"
#include "phases.h"

/* 
 * Note to self: Remember to erase this file so my victims will have no
 * idea what is going on, and so they will all blow up in a
 * spectaculary fiendish explosion. -- Dr. Evil 
 * 中文：给自己的提醒：记得删掉这个文件，让受害者摸不清状况，然后在一场壮观而邪恶的爆炸
 * 中全军覆没。——邪恶博士。
 */

FILE *infile;

int main(int argc, char *argv[])
{
    char *input;

    /* Note to self: remember to port this bomb to Windows and put a 
     * fantastic GUI on it.
     * 中文：给自己的提醒：记得把炸弹移植到 Windows，并配上漂亮的图形界面。
     */

    /* When run with no arguments, the bomb reads its input lines 
     * from standard input.
     * 中文：不带参数运行时，炸弹从标准输入逐行读取输入。
     */
    if (argc == 1) {  
	infile = stdin;
    } 

    /* When run with one argument <file>, the bomb reads from <file> 
     * until EOF, and then switches to standard input. Thus, as you 
     * defuse each phase, you can add its defusing string to <file> and
     * avoid having to retype it.
     * 中文：带一个文件参数运行时，先从该文件读取，直到文件结束（EOF），然后切换到标准输入。
     * 每拆除一关，就可把这一关的输入字符串加入文件，避免反复输入。
     */
    else if (argc == 2) {
	if (!(infile = fopen(argv[1], "r"))) {
	    printf("%s: Error: Couldn't open %s\n", argv[0], argv[1]);
	    exit(8);
	}
    }

    /* You can't call the bomb with more than 1 command line argument.  中文：调用炸弹时最多只能提供一个命令行参数。 */
    else {
	printf("Usage: %s [<input_file>]\n", argv[0]);
	exit(8);
    }

    /* Do all sorts of secret stuff that makes the bomb harder to defuse.  中文：做各种秘密准备，让炸弹更难拆除。 */
    initialize_bomb();

    printf("Welcome to my fiendish little bomb. You have 6 phases with\n");
    printf("which to blow yourself up. Have a nice day!\n");

    /* Hmm...  Six phases must be more secure than one phase!  中文：嗯……六个阶段一定比一个阶段更安全！ */
    input = read_line();             /* Get input                    中文：读取输入。 */
    phase_1(input);                  /* Run the phase                中文：运行当前阶段。 */
    phase_defused();                 /* Drat!  They figured it out!
				      * Let me know how they did it.
     * 中文：可恶！他们破解了！告诉我他们是怎么做到的。
     */
    printf("Phase 1 defused. How about the next one?\n");

    /* The second phase is harder.  No one will ever figure out
     * how to defuse this...
     * 中文：第二阶段更难，绝不会有人知道怎么拆除……
     */
    input = read_line();
    phase_2(input);
    phase_defused();
    printf("That's number 2.  Keep going!\n");

    /* I guess this is too easy so far.  Some more complex code will
     * confuse people.
     * 中文：到目前为止好像太简单了，再复杂一点的代码会让人困惑。
     */
    input = read_line();
    phase_3(input);
    phase_defused();
    printf("Halfway there!\n");

    /* Oh yeah?  Well, how good is your math?  Try on this saucy problem!  中文：哦，是吗？你的数学有多好？来试试这个刺激的问题！ */
    input = read_line();
    phase_4(input);
    phase_defused();
    printf("So you got that one.  Try this one.\n");
    
    /* Round and 'round in memory we go, where we stop, the bomb blows!  中文：在内存中兜兜转转，停错地方，炸弹就爆炸！ */
    input = read_line();
    phase_5(input);
    phase_defused();
    printf("Good work!  On to the next...\n");

    /* This phase will never be used, since no one will get past the
     * earlier ones.  But just in case, make this one extra hard.
     * 中文：这个阶段永远不会被用到，因为没人能通过前面几关。不过以防万一，让它更难一些。
     */
    input = read_line();
    phase_6(input);
    phase_defused();

    /* Wow, they got it!  But isn't something... missing?  Perhaps
     * something they overlooked?  Mua ha ha ha ha!
     * 中文：哇，他们成功了！但是否还缺了点什么……也许他们漏掉了某些东西？哈哈哈哈！
     */
    
    return 0;
}
