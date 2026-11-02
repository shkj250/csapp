/*
 * mm-naive.c - The fastest, least memory-efficient malloc package.
 * 
 * In this naive approach, a block is allocated by simply incrementing
 * the brk pointer.  A block is pure payload. There are no headers or
 * footers.  Blocks are never coalesced or reused. Realloc is
 * implemented directly using mm_malloc and mm_free.
 *
 * NOTE TO STUDENTS: Replace this header comment with your own header
 * comment that gives a high level description of your solution.
 * 中文：mm-naive.c：速度最快但内存利用效率最低的分配器。朴素策略通过增加 brk 指针分配
 * 块；按原文描述，块仅包含 payload，没有块头或块尾，不合并或复用块。realloc 直接调用
 * mm_malloc 和 mm_free 实现。学生须知：请用自己的文件头注释替换本说明，概述你的设计。
 * （原文与当前代码有一处差异：实际起始实现会在 payload 前保存请求大小；此说明不改变代
 * 码。）
 */
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <unistd.h>
#include <string.h>

#include "mm.h"
#include "memlib.h"

/*********************************************************
 * NOTE TO STUDENTS: Before you do anything else, please
 * provide your team information in the following struct.
 * 中文：学生须知：开始其他工作前，请在下面的结构体中填写团队信息。
 ********************************************************/
team_t team = {
    /* Team name  中文：团队名称。 */
    "ateam",
    /* First member's full name  中文：第一位成员的全名。 */
    "Harry Bovik",
    /* First member's email address  中文：第一位成员的电子邮箱。 */
    "bovik@cs.cmu.edu",
    /* Second member's full name (leave blank if none)  中文：第二位成员的全名；没有则留空。 */
    "",
    /* Second member's email address (leave blank if none)  中文：第二位成员的电子邮箱；没有则留空。 */
    ""
};

/* single word (4) or double word (8) alignment  中文：单字（4 字节）或双字（8 字节）对齐。 */
#define ALIGNMENT 8

/* rounds up to the nearest multiple of ALIGNMENT  中文：向上取整到最近的 ALIGNMENT 整数倍。 */
#define ALIGN(size) (((size) + (ALIGNMENT-1)) & ~0x7)


#define SIZE_T_SIZE (ALIGN(sizeof(size_t))) //8

#define GET_SIZE(p)  (*(unsigned int *)(p) & ~0x7)
#define IF_ALLOC(p) (*(unsigned int *)(p) & 0x1)
/* 
 * mm_init - initialize the malloc package.
 * 中文：mm_init：初始化内存分配器。
 * //header//内容+填充(8的倍数)//footer
 * header,footer 8byte 最低位1启用,0空闲
 */
static char*heap_start;
int mm_init(void)
{   
    char*p=mem_sbrk(4096*2);
    if (p==(void*)-1){
        return -1;
    }

    heap_start = p;

    // 整个块大小为 4096，最低位为 0，表示空闲。
    // 头尾各预留 8 字节，但只用其中 4 字节保存信息。
    //header
    *(unsigned int *)p = 4096*2 ;
    //tail
    *(unsigned int *)(p + 4096*2 - 8) = 4096*2 ;

    return 0;
}

/* 
 * mm_malloc - Allocate a block by incrementing the brk pointer.
 *     Always allocate a block whose size is a multiple of the alignment.
 * 中文：mm_malloc：通过增加 brk 指针分配块；块大小始终为对齐单位的整数倍。
 */
void *mm_malloc(size_t size)

{
   
    int total_size=ALIGN(size)+16;
    char*p=heap_start;
    while(p<(char*)mem_heap_hi()){
        unsigned int* header=(unsigned int *)p; 
        int the_size=GET_SIZE(header);
        //find
        if(the_size>=total_size && IF_ALLOC((unsigned int*)p)==0){
            int rest=the_size-total_size;
            
            //header--total--tail-newheader---rest---newtail
            if(rest>=24){
                unsigned int *tail=(unsigned int*)(p+total_size-8);

                unsigned int *new_head=tail+2;
                unsigned int*new_tail=(unsigned int*)(p+the_size-8);
        
                *new_head=rest;
                *header=total_size|1;
                *tail=total_size|1;
                *new_tail=rest;
            }
            else{
                unsigned int *tail=(unsigned int*)(p+the_size-8);
                *header=the_size|1;
                *tail=the_size|1;
            }

           
            return (void*)(p+8);
            
        }
        p=p+the_size;
    }

//finally not find ,extend
    
    
    void *ptr = mem_sbrk(total_size);
    if (ptr == (void *)-1)
        return NULL;
    else {
        *(unsigned int*)ptr = total_size |1 ;
        unsigned int*tail=(unsigned int*)((char*)ptr+total_size-8);
        *tail=total_size|1;
        return (void *)((char *)ptr + 8);
    }




    
    

   
}

/*
 * mm_free - Freeing a block does nothing.
 * 中文：mm_free：释放块时不执行任何操作。
 */
void mm_free(void *ptr)
{
    //set header and tail to unalloc
    unsigned int * header_p=(unsigned int *)ptr-2;
    int size=GET_SIZE(header_p);
    *header_p=size;
    unsigned int *tail_p=(unsigned int *) ((char*)ptr+size-16);
    *tail_p=size;
    
    //merge former
    unsigned int *former_tail=header_p-2;
    // 判断formertail的合法性和空闲
    if(former_tail>mem_heap_lo() && IF_ALLOC(former_tail)==0 ){
        int former_size=GET_SIZE(former_tail);
        int sum_size=size+former_size;
        unsigned int* former_header=(unsigned int*)( (char*)former_tail-former_size+8);
        *former_header=sum_size;
        *tail_p=sum_size;


    }

    //merge next
    unsigned int *next_header=tail_p+2;
    //判断next_header的合法性和空闲
    if(next_header<mem_heap_hi()&&IF_ALLOC(next_header)==0 ){
        int next_size=GET_SIZE(next_header);
        //可能之前merge过了
        int new_size=GET_SIZE(tail_p);
        int sum_size=next_size+new_size;
        unsigned int* new_header=(unsigned int *)( (char*)tail_p-new_size+8);
        unsigned int *next_tail=(unsigned int *)((char*)next_header+next_size-8);
        *new_header=sum_size;
        *next_tail=sum_size;



    }





    

}

/*
 * mm_realloc - Implemented simply in terms of mm_malloc and mm_free
 * 中文：mm_realloc：简单地借助 mm_malloc 和 mm_free 实现。
 */
void *mm_realloc(void *ptr, size_t size)
{

    
    if (ptr==NULL){
        return mm_malloc(size);
    }
    if(size==0){
        mm_free(ptr);
        return NULL;
    }
    int needed_size=ALIGN(size)+16;
    char* old_head=(char*)ptr-8;
    int old_size=GET_SIZE(old_head);
    if(old_size>=needed_size){
        return ptr;
    }
    char*next_head=old_head+old_size;
    if(next_head<mem_heap_hi()){

        int next_size=GET_SIZE(next_head);
        if(IF_ALLOC(next_head)==0&&needed_size<=next_size+old_size){
            *(unsigned int*)old_head=(old_size+next_size)|1;
            char*next_tail=next_head+next_size-8;
            *(unsigned int*)next_tail=(old_size+next_size)|1;
            return ptr;
        }
}

    void *newptr = mm_malloc(size);
    if (newptr == NULL)
        return NULL;  // 失败时保留旧块

    size_t copy_size = old_size - 16;  // 旧块的 payload 容量
    if (size < copy_size)
        copy_size = size;

    memcpy(newptr, ptr, copy_size);
    mm_free(ptr);
    return newptr;

    



}














