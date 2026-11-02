/* 
 * cachelab.h - Prototypes for Cache Lab helper functions
 * 中文：cachelab.h：Cache Lab 辅助函数的原型声明。
 */

#ifndef CACHELAB_TOOLS_H
#define CACHELAB_TOOLS_H

#define MAX_TRANS_FUNCS 100

typedef struct trans_func{
  void (*func_ptr)(int M,int N,int[N][M],int[M][N]);
  char* description;
  char correct;
  unsigned int num_hits;
  unsigned int num_misses;
  unsigned int num_evictions;
} trans_func_t;

/* 
 * printSummary - This function provides a standard way for your cache
 * simulator * to display its final hit and miss statistics
 * 中文：printSummary：为缓存模拟器提供统一的最终命中和不命中统计输出方式。
 */ 
void printSummary(int hits,  /* number of  hits  中文：命中次数。 */
				  int misses, /* number of misses  中文：不命中次数。 */
				  int evictions); /* number of evictions  中文：驱逐次数。 */

/* Fill the matrix with data  中文：用数据填充矩阵。 */
void initMatrix(int M, int N, int A[N][M], int B[M][N]);

/* The baseline trans function that produces correct results.  中文：能产生正确结果的基线转置函数。 */
void correctTrans(int M, int N, int A[N][M], int B[M][N]);

/* Add the given function to the function list  中文：把指定函数加入函数列表。 */
void registerTransFunction(
    void (*trans)(int M,int N,int[N][M],int[M][N]), char* desc);

#endif /* CACHELAB_TOOLS_H  中文：头文件保护宏 CACHELAB_TOOLS_H 结束。 */
