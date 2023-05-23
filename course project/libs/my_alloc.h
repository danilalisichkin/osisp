#ifndef MY_ALLOC_H
#define MY_ALLOC_H

// Лисичкин Д.А. 150502
// субаллокатор памяти (за основу - однонаправленный список)
// последнее изменение: 14.05.23

#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>

struct mem_block
{
    void *addr;             // указатель на начало блока
    size_t size;            // размер блока
    struct mem_block *next; // указатель на след блок
};

struct mem_pool
{
    size_t block_size;
    void *start;                // указатель на начало пула
    struct mem_block *current;  // указатель на текущий используемый блок
};                              // ="голова"

int get_malloc_calls();

int get_free_calls();

// инициализация пула памяти
void mem_pool_init(struct mem_pool *pool, size_t pool_size, size_t block_size);

// инициализация блока в пуле
void mem_block_init(struct mem_pool *pool, size_t size);

// выделение памяти для данных
void *mem_alloc(struct mem_pool *pool, size_t size);

// освобождение памяти
void mem_free(struct mem_pool *pool, void *addr);

// уничтожение пула
void mem_pool_destroy(struct mem_pool *pool);

#endif /* MY_ALLOC_H */
