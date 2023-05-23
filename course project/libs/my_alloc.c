// Лисичкин Д.А. 150502
// субаллокатор памяти (за основу - однонаправленный список)
// последнее изменение: 14.05.23

#include "my_alloc.h"

static int my_ml_counter; // счетчик обращений к malloc()
static int my_fr_counter; // счетчик обращений к free()

int get_malloc_calls()
{
    return my_ml_counter;
}

int get_free_calls()
{
    return my_fr_counter;
}

// Инициализация пула памяти
void mem_pool_init(struct mem_pool *pool, size_t pool_size, size_t block_size)
{
    if (pool_size == 0)
    {
        fprintf(stderr, "Incorrect size of memory space to initialize!\n");
        exit(1);
    }

    // Выделение памяти для пула
    pool->start = malloc(pool_size);
    ++my_ml_counter;
    if (pool->start == NULL)
    {
        fprintf(stderr,"Can't initialize %ld bytes in memory for pool using malloc()!\n", pool_size);
        exit(1);
    }

    // Разбиение пула на блоки заданного размера
    size_t real_block_size = sizeof(struct mem_block) + block_size;
    size_t num_blocks = pool_size / real_block_size;

    struct mem_block *prev_block = NULL;

    for (size_t i = 0; i < num_blocks; i++)
    {
        struct mem_block *block = (struct mem_block *)((char *)pool->start + i * real_block_size);
        block->addr = (char*)block + sizeof(struct mem_block);
        block->size = block_size;
        block->next = NULL;

        pool->block_size = block_size;

        if (prev_block != NULL) prev_block->next = block;

        prev_block = block;
    }

    // Установка текущего блока на первый блок в пуле
    pool->current = (struct mem_block *)pool->start;
}

// Выделение памяти из пула
void *mem_alloc(struct mem_pool *pool, size_t size)
{
    // Поиск блока с достаточным свободным местом
    struct mem_block *current = pool->current;
    while (current != NULL)
    {
        if (current->size >= size && current->size == pool->block_size)
        {
            void *addr = current->addr; // Указатель на начало блока
            current->addr = (void *)((char *)current->addr + size);
            current->size -= size; // Уменьшение свободного места в блоке
            return addr;           // Возвращаем адрес начала блока
        }
        current = current->next;
    }

    // Если подходящий блок не найден
    fprintf(stderr, "No enough memory in the pool!\n");
    return NULL;
}

// Освобождение памяти и возвращение в пул
void mem_free(struct mem_pool *pool, void *addr)
{
    if (pool == NULL || addr == NULL)
    {
        fprintf(stderr, "Can't free not allocated memory!\n");
        return;
    }


    struct mem_block *block = (struct mem_block *)((char *)addr - sizeof(struct mem_block));

    if(block!=pool->current) {
        struct mem_block *prev_block = (struct mem_block *)((char *)addr - pool->block_size - 2*sizeof(struct mem_block));
        prev_block->next = block->next;
    }

    block->next = pool->current;
    block->addr = (void*)((char*)block->addr - (pool->block_size - block->size));
    block->size = pool->block_size;
    pool->current = block;
}

// Освобождение пула памяти и освобождение занятой памяти
void mem_pool_destroy(struct mem_pool *pool)
{
    if (pool != NULL)
    {
        free(pool->start);
        ++my_fr_counter;
        pool->start = NULL;
        pool->current = NULL;
    }
}
