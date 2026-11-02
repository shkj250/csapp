/* 
 * trans.c - Matrix transpose B = A^T
 *
 * Each transpose function must have a prototype of the form:
 * void trans(int M, int N, int A[N][M], int B[M][N]);
 *
 * A transpose function is evaluated by counting the number of misses
 * on a 1KB direct mapped cache with a block size of 32 bytes.
 * 中文：trans.c：矩阵转置 B = A^T。每个转置函数必须采用以下原型：void trans(int M, int
 *  N, int A[N][M], int B[M][N]);。性能通过容量 1KB、块大小 32 字节的直接映射缓存上的不
 * 命中次数评估。
 */ 
#include <stdio.h>
#include "cachelab.h"

int is_transpose(int M, int N, int A[N][M], int B[M][N]);

/* 
 * transpose_submit - This is the solution transpose function that you
 *     will be graded on for Part B of the assignment. Do not change
 *     the description string "Transpose submission", as the driver
 *     searches for that string to identify the transpose function to
 *     be graded. 
 * 中文：transpose_submit：Part B 正式评分的转置函数。不要修改描述字符串 "Transpose
 * submission"，驱动程序用它识别需要评分的函数。
 */
char transpose_submit_desc[] = "Transpose submission";
void transpose_submit(int M, int N, int A[N][M], int B[M][N])
{
}

/* 
 * You can define additional transpose functions below. We've defined
 * a simple one below to help you get started. 
 * 中文：可以在下面定义其他转置函数。已提供一个简单版本，帮助你开始实验。
 */ 

/* 
 * trans - A simple baseline transpose function, not optimized for the cache.
 * 中文：trans：简单的基线转置函数，尚未针对缓存优化。
 */
char trans_desc[] = "Simple row-wise scan transpose";
void trans(int M, int N, int A[N][M], int B[M][N])
{
    int i, j, tmp;

    for (i = 0; i < N; i++) {
        for (j = 0; j < M; j++) {
            tmp = A[i][j];
            B[j][i] = tmp;
        }
    }    

}

/*
 * registerFunctions - This function registers your transpose
 *     functions with the driver.  At runtime, the driver will
 *     evaluate each of the registered functions and summarize their
 *     performance. This is a handy way to experiment with different
 *     transpose strategies.
 * 中文：registerFunctions：向驱动程序注册转置函数。运行时驱动会评估每个已注册函数并汇
 * 总性能，便于比较不同转置策略。
 */
void registerFunctions()
{
    /* Register your solution function  中文：注册你的正式提交函数。 */
    registerTransFunction(transpose_submit, transpose_submit_desc); 

    /* Register any additional transpose functions  中文：注册其他转置函数。 */
    registerTransFunction(trans, trans_desc); 

}

/* 
 * is_transpose - This helper function checks if B is the transpose of
 *     A. You can check the correctness of your transpose by calling
 *     it before returning from the transpose function.
 * 中文：is_transpose：检查 B 是否为 A 的转置。可以在转置函数返回前调用它，检查结果的正
 * 确性。
 */
int is_transpose(int M, int N, int A[N][M], int B[M][N])
{
    int i, j;

    for (i = 0; i < N; i++) {
        for (j = 0; j < M; ++j) {
            if (A[i][j] != B[j][i]) {
                return 0;
            }
        }
    }
    return 1;
}

