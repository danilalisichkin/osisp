#ifndef OTHER_MY_ALLOC_H
#define OTHER_MY_ALLOC_H

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>

#include "statistic.h"

extern statistic stats;

enum alloc_strategy
{
    FIRST_FIT,
    BEST_FIT,
    WORST_FIT
};

typedef struct mem_block
{
    void *addr;             
    size_t size;            
} mem_block;

typedef struct mem_pool
{
    enum alloc_strategy strategy; 

    void *start;                     
    void *used;                      

    size_t num_of_available_blocks;  
    size_t num_of_used_blocks;       
    size_t size_of_available_space;  
    size_t size_of_used_space;       

} mem_pool;

char *get_pool_stats(mem_pool *pool);

void mem_pool_init(mem_pool *pool, size_t pool_size, enum alloc_strategy strategy);

void mem_block_init(mem_pool *pool, size_t size);

void *mem_alloc(mem_pool *pool, size_t size);

void *mem_calloc(mem_pool *pool, size_t num, size_t size);

void *mem_realloc(mem_pool *pool, void *addr, size_t size);

void mem_free(mem_pool *pool, void *addr);

void mem_pool_destroy(struct mem_pool *pool);

#endif /* OTHER_MY_ALLOC_H */
