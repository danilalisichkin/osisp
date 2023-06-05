#include "my_alloc.h"

statistic stats;

char *get_pool_stats(mem_pool *pool)
{
    char *buffer = malloc(256 * sizeof(char));
    sprintf(buffer, "PS: %ld | AS: %ld | US: %ld | UB: %ld | AB: %ld",
            pool->size_of_available_space + pool->size_of_used_space,
            pool->size_of_available_space, pool->size_of_used_space,
            pool->num_of_used_blocks, pool->num_of_available_blocks);
    return buffer;
}

void mem_pool_init(mem_pool *pool, size_t pool_size, enum alloc_strategy strategy)
{
    if (pool_size == 0) {
        fprintf(stderr, "Incorrect size of memory space to initialize!\n");
        exit(1);
    }

    pool->start = malloc(pool_size);
    if (pool->start == NULL) {
        fprintf(stderr, "Can't initialize %ld bytes in memory for pool using malloc()!\n", pool_size);
        exit(1);
    }

    pool->used = (void *)((char *)pool->start + sizeof(struct mem_pool));

    pool->strategy = strategy;

    pool->num_of_used_blocks = 0;         
    pool->num_of_available_blocks = 0;    
    pool->size_of_used_space = sizeof(struct mem_pool);     
    pool->size_of_available_space = pool_size - sizeof(struct mem_pool); 
}

void mem_block_init(mem_pool *pool, size_t size)
{
    size_t real_block_size = sizeof(struct mem_block) + size;

    if (real_block_size > pool->size_of_available_space) {
        fprintf(stderr, "No enough memory to create a new memory block in a pool!\n");
        exit(1);
    }

    struct mem_block *new_block;
    new_block = (struct mem_block *) ((char *) pool->used);
    new_block->addr = (void *) ((char *) new_block + sizeof(struct mem_block));
    new_block->size = size;

    ++pool->num_of_used_blocks;
    pool->size_of_used_space += sizeof(struct mem_block);
    pool->size_of_available_space -= sizeof(struct mem_block);
    pool->used = (void *) ((char *) pool->used + real_block_size);
}

void *mem_alloc(mem_pool *pool, size_t size)
{
    if (size == 0) {
        return NULL;
    }

    struct mem_block *curr;
    void *data_adr;

    if (pool->num_of_available_blocks > 0) {

        curr = (struct mem_block *) ((char *) pool->start + sizeof(struct mem_pool));
        struct mem_block *best_fit_block = NULL;
        struct mem_block *worst_fit_block = NULL;
        size_t curr_free_space = 0;
        size_t best_fit_size = pool->size_of_available_space;
        size_t worst_fit_size = 0;
        size_t occupied_size = 0;

        switch (pool->strategy) {
            case FIRST_FIT: {
                while (curr->addr != NULL)
                {
                    curr_free_space = curr->size - ((char *) curr->addr - (char *) curr - sizeof(struct mem_block));
                    if (curr_free_space < size) {
                        curr = (void *) ((char *) curr + sizeof(struct mem_block) + curr->size);
                        continue;
                    }
                    if (curr_free_space == curr->size) {

                        data_adr = curr->addr;
                        curr->addr = (void *) ((char *) curr->addr + size);
                        occupied_size = size;
                    }
                    else if (curr_free_space >= size + sizeof(struct mem_block)) {

                        struct mem_block *new_block = (struct mem_block *) (curr->addr);
                        data_adr = (void *) ((char *) curr->addr + sizeof(struct mem_block));

                        new_block->addr = (void *) ((char *) data_adr + size);
                        new_block->size = curr_free_space - sizeof(struct mem_block);

                        curr->size -= curr_free_space;
                        occupied_size = size + sizeof(struct mem_block);
                    } else {
                        curr = (void *) ((char *) curr + sizeof(struct mem_block) + curr->size);
                        continue;
                    }

                    ++pool->num_of_used_blocks;
                    pool->size_of_used_space += occupied_size;
                    pool->size_of_available_space -= occupied_size;

                    return data_adr;
                }
                break;
            }
            case BEST_FIT: {
                while (curr->addr != NULL)
                {
                    curr_free_space = curr->size - ((char *) curr->addr - (char *) curr - sizeof(struct mem_block));
                    if (curr_free_space < size) {
                        curr = (void *) ((char *) curr + sizeof(struct mem_block) + curr->size);
                        continue;
                    }
                    if (curr_free_space < best_fit_size &&
                        curr_free_space == curr->size) {

                        best_fit_block = curr;
                        best_fit_size = curr_free_space;

                        if (curr_free_space == size + sizeof(struct mem_block)) {
                            break;
                        }
                    }
                    else if (curr_free_space - sizeof(struct mem_block) >= size &&
                             curr_free_space - sizeof(struct mem_block) < best_fit_size) {

                        best_fit_block = curr;
                        best_fit_size = curr_free_space - sizeof(struct mem_block);
                    }
                    curr = (void *) ((char *) curr + sizeof(struct mem_block) + curr->size);
                }

                if (best_fit_block->addr != NULL) {
                    curr_free_space = best_fit_block->size - ((char *) best_fit_block->addr - 
                           	      (char *) best_fit_block - sizeof(struct mem_block));
                    if (curr_free_space == best_fit_block->size) {
                        data_adr = best_fit_block->addr;
                        best_fit_block->addr = (void *) ((char *) best_fit_block->addr + size);

                        --pool->num_of_available_blocks;
                        occupied_size = size;
                    }
                    else {
                        struct mem_block *new_block = (struct mem_block *) (best_fit_block->addr);
                        data_adr = (void *) ((char *) best_fit_block->addr + sizeof(struct mem_block));

                        new_block->addr = (void *) ((char *) data_adr + size);
                        new_block->size = curr_free_space - sizeof(struct mem_block);

                        best_fit_block->size -= curr_free_space;
                        occupied_size = size + sizeof(struct mem_block);
                    }

                    ++pool->num_of_used_blocks;
                    pool->size_of_used_space += occupied_size;
                    pool->size_of_available_space -= occupied_size;

                    return data_adr;
                }
                break;
            }
            case WORST_FIT: {
                while (curr->addr != NULL)
                {
                    curr_free_space = curr->size - ((char *) curr->addr - (char *) curr - sizeof(struct mem_block));
                    if (curr_free_space < size) {
                        curr = (void *) ((char *) curr + sizeof(struct mem_block) + curr->size);
                        continue;
                    }
                    if (curr_free_space > worst_fit_size &&
                        curr_free_space == curr->size) {

                        worst_fit_block = curr;
                        worst_fit_size = curr_free_space;
                    }
                    else if (curr_free_space >= size + sizeof(struct mem_block) &&
                             curr_free_space > worst_fit_size + sizeof(struct mem_block)) {

                        worst_fit_block = curr;
                        worst_fit_size = curr_free_space - sizeof(struct mem_block);
                    }
                    curr = (void *) ((char *) curr + sizeof(struct mem_block) + curr->size);
                }

                if (worst_fit_block->addr != NULL) {

                    curr_free_space = worst_fit_block->size - ((char *) worst_fit_block->addr -
                                      (char *) worst_fit_block - sizeof(struct mem_block));
                    if (curr_free_space == worst_fit_block->size) {
                        data_adr = worst_fit_block->addr;
                        worst_fit_block->addr = (void *) ((char *) worst_fit_block->addr + size);

                        --pool->num_of_available_blocks;
                        occupied_size = size;
                    }
                    else {
                        struct mem_block *new_block = (struct mem_block *) (worst_fit_block->addr);
                        data_adr = (void *) ((char *) worst_fit_block->addr + sizeof(struct mem_block));

                        new_block->addr = (void *) ((char *) data_adr + size);
                        new_block->size = curr_free_space - sizeof(struct mem_block);

                        worst_fit_block->size -= curr_free_space;
                        occupied_size = size + sizeof(struct mem_block);
                    }

                    ++pool->num_of_used_blocks;
                    pool->size_of_used_space += occupied_size;
                    pool->size_of_available_space -= occupied_size;

                    return data_adr;
                }
                break;
            }
        }
    }
    curr = (struct mem_block *) pool->used;

    mem_block_init(pool, size);

    data_adr = (void *) ((char *) curr + sizeof(struct mem_block));
    curr->addr = (void *) ((char *) data_adr + size);

    pool->size_of_available_space -= size;
    pool->size_of_used_space += size;

    return data_adr;
}

void *mem_calloc(mem_pool *pool, size_t num, size_t size)
{
    size_t total_size = num * size;

    void *addr = mem_alloc(pool, total_size);

    if (addr != NULL) {
        memset(addr, 0, total_size);
    }

    return addr;
}

void *mem_realloc(mem_pool *pool, void *addr, size_t size)
{
    if (addr == NULL) {
        return mem_alloc(pool, size);
    }

    if (size == 0) {
        mem_free(pool, addr);
        return NULL;
    }

    struct mem_block *block = (struct mem_block *) ((char *) addr - sizeof(struct mem_block));

    if (block->size >= size) {
        return addr;
    }

    void *new_addr = mem_alloc(pool, size);
    if (new_addr != NULL) {
        memcpy(new_addr, addr, block->size);
        mem_free(pool, addr);
    }
    return new_addr;
}

void mem_free(mem_pool *pool, void *addr)
{
    if (pool == NULL || addr == NULL) {
        fprintf(stderr, "Can't free not allocated memory!\n");
        return;
    }

    struct mem_block *block = (struct mem_block *) ((char *) addr - sizeof(struct mem_block));
    struct mem_block *right_block = (struct mem_block *) ((char *) addr + block->size);

    block->addr = addr;
    size_t freed_space = block->size - ((char *) block->addr - (char *) block - sizeof(struct mem_block));

    if (right_block->addr == (void *) ((char *) right_block + sizeof(struct mem_block))) {
        block->size += right_block->size + sizeof(struct mem_block);
        freed_space += sizeof(struct mem_block);
        --pool->num_of_available_blocks;
    }

    pool->size_of_available_space += freed_space;
    pool->size_of_used_space -= freed_space;
    ++pool->num_of_available_blocks;
    --pool->num_of_used_blocks;
}

void mem_pool_destroy(mem_pool *pool)
{
    if (pool != NULL) {
        free(pool->start);

        memset(pool, 0, sizeof(struct mem_pool));
    }
}
