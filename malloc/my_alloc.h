#ifndef MY_ALLOC_H
#define MY_ALLOC_H

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <string.h>

#define MEMORY_SIZE 1024 // Размер общего блока памяти
#define MIN_BLOCK_SIZE 16 // Минимальный размер блока

typedef struct Block {
    size_t size;
    void* address;
    struct Block* prev;
    struct Block* next;
} Block;

int get_malloc_calls();

int get_free_calls();

void* suballocator_malloc(size_t size);

void suballocator_free(void* ptr);

void* suballocator_realloc(void* ptr, size_t size);

void suballocator_init();

void suballocator_destroy();

int main();

#endif /* MY_ALLOC_H */