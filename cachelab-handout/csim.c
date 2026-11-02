#include "cachelab.h"
#include "stdio.h"
#include <stdlib.h>

/*
 * csim.c - Starter file for your Cache Lab simulator (Part A).
 * 中文：Cache Lab Part A 缓存模拟器的起始文件，由你完成实现。
 */
int main(int argc,char *argv[])
{
    if (argc!=9){
        printf("用法：./csim -s 数值 -E 数值 -b 数值 -t 文件名\n");
        return 1;

    }
    int s=atoi(argv[2]);
    int S=1<<s;
    int E=atoi(argv[4]);
    int b=atoi(argv[6]);
    char *trace_file=argv[8];
    FILE *fp=fopen(trace_file,"r");
    if (fp==NULL){
        printf("fail to open the trace file");
        return 1;
    }
    int hits=0;
    int misses=0;
    int evictions=0;
    char line[255];
    char op;
    unsigned long long address;
    int size;
    typedef struct {
        int valid;
        int last_used;
        unsigned long long tag;


    }cache;
    cache caches[S][E];
    //init caches
    for (int i=0 ;i<S;i++){
        for (int j=0;j<E;j++){
            caches[i][j].valid=0;
            caches[i][j].last_used=0;
        }
    }
    //address:  tag /s bit for set index /b bit for block offset
    //cache    2^s set,each set has E line,each line is valid /tag/2^b bit
    while (fgets(line,sizeof(line),fp) !=NULL){
        if(sscanf(line," %c %llx,%d",&op,&address,&size)==3){
            // op is I continue
            if (op=='I'){
                continue;
            }

            // op is M, access the memory 2 times
            else if(op=='M'){
                int set_index=(address>>b)&((1ULL<<s)-1);
                unsigned long long tag=address>>(s+b);
                int found_flag=0;
                int max_last_used=0;
                int have_place=0;
                cache * empty_cache;
                cache * replaced_cache;
                for(int i =0;i<E;i++){
                    cache *thecache=&caches[set_index][i];
                    thecache->last_used++;
                    if (thecache->valid==0){
                        have_place=1;
                        empty_cache=thecache;
                    }
                    if (thecache->valid==1 && thecache->tag==tag){
                        hits+=2;
                        thecache->last_used=0;
                        found_flag=1;
                    }

                }
                //not hit, we need to write the cache
                //if have empty place
                if (have_place==1 &&found_flag==0){
                    misses++;
                    hits++;
                    empty_cache->last_used=0;
                    empty_cache->tag=tag;
                    empty_cache->valid=1;

                }
                // dont have empty place ,need to replace
                else if (have_place==0 &&found_flag==0){
                    evictions++;
                    misses++;
                    hits++;
                    //find the max last_used
                    for(int i=0;i<E;i++){
                        if (max_last_used<caches[set_index][i].last_used){
                            replaced_cache=&caches[set_index][i];
                            max_last_used=caches[set_index][i].last_used;

                        }
                    }
                    //replace
                    replaced_cache->last_used=0;
                    replaced_cache->valid=1;
                    replaced_cache->tag=tag;



                }               
            }

            //op is L or S, access the memory 1 times
            else{
                int set_index=(address>>b)&((1ULL<<s)-1);
                unsigned long long tag=address>>(s+b);
                int found_flag=0;
                int max_last_used=0;
                int have_place=0;
                cache * empty_cache;
                cache * replaced_cache;
                for(int i =0;i<E;i++){
                    cache *thecache=&caches[set_index][i];
                    thecache->last_used++;
                    if (thecache->valid==0){
                        have_place=1;
                        empty_cache=thecache;
                    }
                    if (thecache->valid==1 && thecache->tag==tag){
                        hits++;
                        thecache->last_used=0;
                        found_flag=1;
                    }

                }
                //not hit, we need to write the cache
                //if have empty place
                if (have_place==1 &&found_flag==0){
                    misses++;
                    empty_cache->last_used=0;
                    empty_cache->tag=tag;
                    empty_cache->valid=1;

                }
                // dont have empty place ,need to replace
                else if (have_place==0 &&found_flag==0){
                    evictions++;
                    misses++;
                    //find the max last_used
                    for(int i=0;i<E;i++){
                        if (max_last_used<caches[set_index][i].last_used){
                            replaced_cache=&caches[set_index][i];
                            max_last_used=caches[set_index][i].last_used;

                        }
                    }
                    //replace
                    replaced_cache->last_used=0;
                    replaced_cache->valid=1;
                    replaced_cache->tag=tag;



                }


            }

        }

    }
    printSummary(hits,misses,evictions);
    return 0;

}
