/********************************************************
 * Kernels to be optimized for the CS:APP Performance Lab
 * 中文：CS:APP Performance Lab 中需要优化的计算核心。
 ********************************************************/

#include <stdio.h>
#include <stdlib.h>
#include "defs.h"

/* 
 * Please fill in the following team struct 
 * 中文：请填写下面的团队信息结构体。
 */
team_t team = {
    "bovik",              /* Team name  中文：团队名称。 */

    "Harry Q. Bovik",     /* First member full name  中文：第一位成员的全名。 */
    "bovik@nowhere.edu",  /* First member email address  中文：第一位成员的电子邮箱。 */

    "",                   /* Second member full name (leave blank if none)  中文：第二位成员的全名；没有则留空。 */
    ""                    /* Second member email addr (leave blank if none)  中文：第二位成员的电子邮箱；没有则留空。 */
};

/***************
 * ROTATE KERNEL
 * 中文：旋转计算核心。
 ***************/

/******************************************************
 * Your different versions of the rotate kernel go here
 * 中文：在这里放置旋转核心的不同实现版本。
 ******************************************************/

/* 
 * naive_rotate - The naive baseline version of rotate 
 * 中文：naive_rotate：旋转操作的朴素基线实现。
 */
char naive_rotate_descr[] = "naive_rotate: Naive baseline implementation";
void naive_rotate(int dim, pixel *src, pixel *dst) 
{
    int i, j;

    for (i = 0; i < dim; i++)
	for (j = 0; j < dim; j++)
	    dst[RIDX(dim-1-j, i, dim)] = src[RIDX(i, j, dim)];
}

/* 
 * rotate - Your current working version of rotate
 * IMPORTANT: This is the version you will be graded on
 * 中文：rotate：你当前采用的旋转实现。重要：正式评分使用这个版本。
 */
char rotate_descr[] = "rotate: Current working version";
void rotate(int dim, pixel *src, pixel *dst) 
{
    int i, j;
    int a,b;

    for (i = 0; i < dim-8; i+=8)
	for (j = 0; j < dim-8; j+=8){

        for (a=0;a<8;a++)
        for (b=0;b<8;b++){
            
            dst[RIDX(dim-1-(j+b), i+a, dim)] = src[RIDX(i+a, j+b, dim)];
    }

    }

    int x=i;
    int y=j;

    for (; i < dim; i++)
	for (j = 0; j < dim; j++)
	    dst[RIDX(dim-1-j, i, dim)] = src[RIDX(i, j, dim)];
    

    for (i = 0; i < x; i++)
	for (j = y; j < dim; j++)
	    dst[RIDX(dim-1-j, i, dim)] = src[RIDX(i, j, dim)];



}


/*********************************************************************
 * register_rotate_functions - Register all of your different versions
 *     of the rotate kernel with the driver by calling the
 *     add_rotate_function() for each test function. When you run the
 *     driver program, it will test and report the performance of each
 *     registered test function.  
 * 中文：register_rotate_functions：为每个测试函数调用 add_rotate_function()，将不同旋
 * 转版本注册到驱动程序。运行驱动时会测试并报告每个已注册函数的性能。
 *********************************************************************/

void register_rotate_functions() 
{
    add_rotate_function(&naive_rotate, naive_rotate_descr);   
    add_rotate_function(&rotate, rotate_descr);   
    /* ... Register additional test functions here  中文：在此注册其他测试函数。 */
}


/***************
 * SMOOTH KERNEL
 * 中文：平滑计算核心。
 **************/

/***************************************************************
 * Various typedefs and helper functions for the smooth function
 * You may modify these any way you like.
 * 中文：用于 smooth 的类型定义和辅助函数，可以按需修改。
 **************************************************************/

/* A struct used to compute averaged pixel value  中文：用于计算像素平均值的结构体。 */
typedef struct {
    int red;
    int green;
    int blue;
    int num;
} pixel_sum;

/* Compute min and max of two integers, respectively  中文：分别计算两个整数中的较小值和较大值。 */
static int min(int a, int b) { return (a < b ? a : b); }
static int max(int a, int b) { return (a > b ? a : b); }

/* 
 * initialize_pixel_sum - Initializes all fields of sum to 0 
 * 中文：initialize_pixel_sum：把 sum 的所有字段初始化为 0。
 */
static void initialize_pixel_sum(pixel_sum *sum) 
{
    sum->red = sum->green = sum->blue = 0;
    sum->num = 0;
    return;
}

/* 
 * accumulate_sum - Accumulates field values of p in corresponding 
 * fields of sum 
 * 中文：accumulate_sum：把像素 p 的各字段累加到 sum 的对应字段。
 */
static void accumulate_sum(pixel_sum *sum, pixel p) 
{
    sum->red += (int) p.red;
    sum->green += (int) p.green;
    sum->blue += (int) p.blue;
    sum->num++;
    return;
}

/* 
 * assign_sum_to_pixel - Computes averaged pixel value in current_pixel 
 * 中文：assign_sum_to_pixel：计算像素平均值，写入 current_pixel。
 */
static void assign_sum_to_pixel(pixel *current_pixel, pixel_sum sum) 
{
    current_pixel->red = (unsigned short) (sum.red/sum.num);
    current_pixel->green = (unsigned short) (sum.green/sum.num);
    current_pixel->blue = (unsigned short) (sum.blue/sum.num);
    return;
}

/* 
 * avg - Returns averaged pixel value at (i,j) 
 * 中文：avg：返回位置 (i,j) 的平均像素值。
 */
static pixel avg(int dim, int i, int j, pixel *src) 
{
    int ii, jj;
    pixel_sum sum;
    pixel current_pixel;

    initialize_pixel_sum(&sum);
    for(ii = max(i-1, 0); ii <= min(i+1, dim-1); ii++) 
	for(jj = max(j-1, 0); jj <= min(j+1, dim-1); jj++) 
	    accumulate_sum(&sum, src[RIDX(ii, jj, dim)]);

    assign_sum_to_pixel(&current_pixel, sum);
    return current_pixel;
}

/******************************************************
 * Your different versions of the smooth kernel go here
 * 中文：在这里放置平滑核心的不同实现版本。
 ******************************************************/

/*
 * naive_smooth - The naive baseline version of smooth 
 * 中文：naive_smooth：平滑操作的朴素基线实现。
 */
char naive_smooth_descr[] = "naive_smooth: Naive baseline implementation";
void naive_smooth(int dim, pixel *src, pixel *dst) 
{
    int i, j;

    for (i = 0; i < dim; i++)
	for (j = 0; j < dim; j++)
	    dst[RIDX(i, j, dim)] = avg(dim, i, j, src);
}

/*
 * smooth - Your current working version of smooth. 
 * IMPORTANT: This is the version you will be graded on
 * 中文：smooth：你当前采用的平滑实现。重要：正式评分使用这个版本。
 */
char smooth_descr[] = "smooth: Current working version";
void smooth(int dim, pixel *src, pixel *dst) 
{
    int i,j;
    //边和角
    for (i=0;i<dim;i++){
        dst[RIDX(i,0,dim)]=avg(dim,i,0,src);
        dst[RIDX(i,dim-1,dim)]=avg(dim,i,dim-1,src);
    }
    for(j=1;j<dim-1;j++){
        dst[RIDX(0,j,dim)]=avg(dim,0,j,src);
        dst[RIDX(dim-1,j,dim)]=avg(dim,dim-1,j,src);
    }

    //中间的一大块 num=9
    for (i=1;i<dim-1;i++){
        for(j=1;j<dim-1;j++){
            int sum_red,sum_green,sum_blue;

            sum_red=src[RIDX(i-1,j-1,dim)].red+src[RIDX(i-1,j,dim)].red+src[RIDX(i-1,j+1,dim)].red
            +src[RIDX(i,j-1,dim)].red+src[RIDX(i,j,dim)].red+src[RIDX(i,j+1,dim)].red
            +src[RIDX(i+1,j-1,dim)].red+src[RIDX(i+1,j,dim)].red+src[RIDX(i+1,j+1,dim)].red
            ;
            dst[RIDX(i,j,dim)].red=sum_red/9;

            sum_green=src[RIDX(i-1,j-1,dim)].green+src[RIDX(i-1,j,dim)].green+src[RIDX(i-1,j+1,dim)].green
            +src[RIDX(i,j-1,dim)].green+src[RIDX(i,j,dim)].green+src[RIDX(i,j+1,dim)].green
            +src[RIDX(i+1,j-1,dim)].green+src[RIDX(i+1,j,dim)].green+src[RIDX(i+1,j+1,dim)].green
            ;
            dst[RIDX(i,j,dim)].green=sum_green/9;

            sum_blue=src[RIDX(i-1,j-1,dim)].blue+src[RIDX(i-1,j,dim)].blue+src[RIDX(i-1,j+1,dim)].blue
            +src[RIDX(i,j-1,dim)].blue+src[RIDX(i,j,dim)].blue+src[RIDX(i,j+1,dim)].blue
            +src[RIDX(i+1,j-1,dim)].blue+src[RIDX(i+1,j,dim)].blue+src[RIDX(i+1,j+1,dim)].blue
            ;
            dst[RIDX(i,j,dim)].blue=sum_blue/9;


        }

        
    }
}


/********************************************************************* 
 * register_smooth_functions - Register all of your different versions
 *     of the smooth kernel with the driver by calling the
 *     add_smooth_function() for each test function.  When you run the
 *     driver program, it will test and report the performance of each
 *     registered test function.  
 * 中文：register_smooth_functions：为每个测试函数调用 add_smooth_function()，将不同平
 * 滑版本注册到驱动程序。运行驱动时会测试并报告每个已注册函数的性能。
 *********************************************************************/

void register_smooth_functions() {
    add_smooth_function(&smooth, smooth_descr);
    add_smooth_function(&naive_smooth, naive_smooth_descr);
    /* ... Register additional test functions here  中文：在此注册其他测试函数。 */
}

