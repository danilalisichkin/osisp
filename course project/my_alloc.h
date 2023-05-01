// Лисичкин Д.А. 150502
// субаллокатор памяти (за основу - однонаправленный список)
// последнее изменение: 30.04.23

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
    void *start;                // указатель на начало пула
    struct mem_block *current;  // указатель на текущий используемый блок
};                              // ="голова"

int get_malloc_calls();

// инициализация пула памяти
void mem_pool_init(struct mem_pool *pool, size_t size);

// инициализация блока в пуле
void mem_block_init(struct mem_pool *pool, size_t size);

// выделение памяти для данных
void* mem_alloc(struct mem_pool *pool, size_t size);

// освобождение памяти
void mem_free(struct mem_pool *pool, void *addr);

// уничтожение пула
void mem_pool_destroy(struct mem_pool *pool);