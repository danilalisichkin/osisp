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

// инициализация пула памяти
void mem_pool_init(struct mem_pool *pool, size_t size)
{
    if (!size) {
        perror("Incorrect size of memory space to initialize!\n");
        exit(1);
    }
    // получаем адрес начала пула
    pool->start = malloc(size);
    if (pool->start == NULL) {
        perror("Can't initialize required space in memory using malloc()!\n");
        exit(1);
    }
    my_ml_counter++;
    // получаем адрес текущего блока
    pool->current = (struct mem_block*)pool->start;
    pool->current->addr = (char*)pool->start + sizeof(struct mem_block);
    pool->current->size = size - sizeof(struct mem_block);
    pool->current->next = NULL;
}

// пул памяти и размер нужного блока
void mem_block_init(struct mem_pool *pool, size_t size)
{
    // определяем нужный размер (=размер блока + размер данных)
    size_t block_size = size + sizeof(struct mem_block);
    // адресуемся на пустое место в памяти с нужным размером
    void *block_addr = malloc(block_size);
    my_ml_counter++;
    struct mem_block *new_block = (struct mem_block*)block_addr;
    new_block->addr = (char*)block_addr + sizeof(struct mem_block);
    new_block->size = block_size - sizeof(struct mem_block);
    // создаем новую "голову"
    new_block->next = pool->current;
    pool->current = new_block;
}

void* mem_alloc(struct mem_pool *pool, size_t size) {
    // адресуемся на текущий инициализированный блок памяти
    struct mem_block *current = pool->current;
    // ищем пустной блок в пуле нужного размера
    while (current != NULL) {
        if (current->size >= size) {
            void *addr = current->addr;        // указатель на начало инициализированного блока
            current->addr = (void*)((char*)current->addr + size);
            current->size -= size;             // уменьшаем свободное место в блоке
            return addr;                       // нашли - будем работать с ним!
        }
        current = current->next;
    }
    // если подходящий блок не найден - создаем его
    mem_block_init(pool, size);
    return pool->current->addr;
}

void mem_free(struct mem_pool *pool, void *addr)
{
    if (pool == NULL || addr == NULL) {
        perror("Can't free not allocated memory!\n");
        return;
    }
    // вычисляем положение блока в пуле

    struct mem_block *block = (struct mem_block*)((char*)addr);

    block->next = NULL;
    if (pool->current == NULL) {
        pool->current = block;
    } else {
        struct mem_block *current = pool->current;
        while (current->next != NULL) {
            current = current->next;
        }
        current->next = block;
    }
}

void mem_pool_destroy(struct mem_pool *pool) {
    if (pool->start != NULL) {
        free(pool->start);
        my_fr_counter++;
    }
}
