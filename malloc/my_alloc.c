// Лисичкин Д.А. 150502
// субаллокатор памяти (за основу - однонаправленный список)
// последнее изменение: 14.05.23

#include "my_alloc.h"

static int my_ml_counter; // счетчик обращений к malloc()
static int my_fr_counter; // счетчик обращений к free()

Block* freeList = NULL; // Список свободных блоков
Block* allocatedList = NULL; // Список выделенных блоков
sem_t mutex; // Семафор для синхронизации доступа к freeList и allocatedList

int get_malloc_calls()
{
    return my_ml_counter;
}

int get_free_calls()
{
    return my_fr_counter;
}

void* suballocator_malloc(size_t size) {
    if (size == 0) {
        return NULL;
    }

    size_t blockSize = sizeof(Block) + size; // Размер всего блока, включая заголовок
    blockSize = (blockSize + MIN_BLOCK_SIZE - 1) / MIN_BLOCK_SIZE * MIN_BLOCK_SIZE; // Выравнивание размера блока

    sem_wait(&mutex); // Захват семафора для доступа к freeList и allocatedList

    Block* currBlock = freeList;
    Block* prevBlock = NULL;

    while (currBlock != NULL) {
        if (currBlock->size >= blockSize) { // Найден блок достаточного размера
            if (currBlock->size - blockSize >= sizeof(Block) + MIN_BLOCK_SIZE) {
                // Делим блок на два, если остается достаточно места
                Block* newBlock = (Block*)((char*)currBlock->address + blockSize);
                newBlock->size = currBlock->size - blockSize - sizeof(Block);
                newBlock->address = (char*)currBlock->address + blockSize;
                newBlock->prev = currBlock;
                newBlock->next = currBlock->next;

                if (currBlock->next != NULL) {
                    currBlock->next->prev = newBlock;
                }

                currBlock->size = blockSize;
                currBlock->next = newBlock;
            }

            // Удаляем блок из freeList и добавляем в allocatedList
            if (prevBlock != NULL) {
                prevBlock->next = currBlock->next;
            } else {
                freeList = currBlock->next;
            }

            if (currBlock->next != NULL) {
                currBlock->next->prev = prevBlock;
            }

            currBlock->prev = NULL;
            currBlock->next = allocatedList;

            if (allocatedList != NULL) {
                allocatedList->prev = currBlock;
            }

            allocatedList = currBlock;

            sem_post(&mutex); // Освобождение семафора

            return currBlock->address; // Возвращаем адрес начала блока
        }

        prevBlock = currBlock;
        currBlock = currBlock->next;
    }

    sem_post(&mutex); // Освобождение семафора

    return NULL; // Не удалось найти блок достаточного размера
}

void suballocator_free(void* ptr) {
    if (ptr == NULL) {
        return;
    }

    sem_wait(&mutex); // Захват семафора для доступа к freeList и allocatedList

    Block* block = allocatedList;

    while (block != NULL) {
        if (block->address == ptr) { // Найден блок для освобождения
            // Удаляем блок из allocatedList и добавляем в freeList
            if (block->prev != NULL) {
                block->prev->next = block->next;
            } else {
                allocatedList = block->next;
            }

            if (block->next != NULL) {
                block->next->prev = block->prev;
            }

            block->prev = NULL;
            block->next = freeList;

            if (freeList != NULL) {
                freeList->prev = block;
            }

            freeList = block;

            sem_post(&mutex); // Освобождение семафора

            return;
        }

        block = block->next;
    }

    sem_post(&mutex); // Освобождение семафора
}

void* suballocator_calloc(size_t num, size_t size) {
    size_t totalSize = num * size;
    void* ptr = suballocator_malloc(totalSize);

    if (ptr != NULL) {
        memset(ptr, 0, totalSize);
    }

    return ptr;
}

void* suballocator_realloc(void* ptr, size_t size) {
    if (ptr == NULL) {
        return suballocator_malloc(size);
    }

    if (size == 0) {
        suballocator_free(ptr);
        return NULL;
    }

    Block* block = allocatedList;

    while (block != NULL) {
        if (block->address == ptr) { // Найден блок для перераспределения
            size_t currentSize = block->size - sizeof(Block);

            if (size <= currentSize) {
                return ptr; // Новый размер меньше или равен текущему размеру, возвращаем исходный указатель
            }

            void* newPtr = suballocator_malloc(size);

            if (newPtr != NULL) {
                memcpy(newPtr, ptr, currentSize); // Копируем данные из старой области памяти в новую
                suballocator_free(ptr); // Освобождаем старую область памяти
            }

            return newPtr;
        }

        block = block->next;
    }

    return NULL; // Не удалось найти блок для перераспределения
}

void suballocator_init() {
    freeList = (Block*)sbrk(MEMORY_SIZE); // Выделяем блок памяти с помощью системного вызова sbrk()

    if (freeList == (void*)-1) {
        perror("Error allocating memory");
        exit(1);
    }

    freeList->size = MEMORY_SIZE - sizeof(Block);
    freeList->address = freeList + 1;
    freeList->prev = NULL;
    freeList->next = NULL;

    sem_init(&mutex, 0, 1); // Инициализация семафора
}

void suballocator_destroy() {
    sem_destroy(&mutex); // Уничтожение семафора
}