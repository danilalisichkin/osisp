#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <sys/sem.h>
#include <time.h>

#define QUEUE_MAX_SIZE 10
#define MAX_CHILD 10

pid_t consumers[MAX_CHILD];
int cons_counter;
pid_t producers[MAX_CHILD];
int prod_counter;

pid_t parent_PID;

typedef struct {
    char type;
    short hash;
    char size;
    char *data;
} message;

typedef struct {
    message *head;
    message *tail;
    message buffer[QUEUE_MAX_SIZE];

    int counter_added;
    int counter_extracted;
} ring_queue;

// Алгоритм хэширования FNV-1
uint16_t fnv1_hash(const void *data, size_t size) {
    const uint8_t *bytes = (const uint8_t *)data;
    uint16_t hash = 0x811C;
    for (size_t i = 0; i < size; ++i) {
        hash = (hash * 0x0101) ^ bytes[i];
    }
    return hash;
}
char init_type(char size)
{
    return size >= 128 ? 1 : 0;
}

char init_size()
{
    char size = 0;
    while (size == 0) size = rand() % 257;
    if (size == 256) {
        size = 0;
    }
    return size;
}

char* init_data(char data_size)
{
    if (data_size == 0) data_size = 256;
    char* new_data = (char*)malloc(data_size*sizeof(char));
    srand(time(NULL));

    for (int i = 0; i < data_size-1; i++) {
        new_data[i] = (rand() % 26) + 'a';
    }
    new_data[data_size] = '\0';

    return new_data;
}

message* create_message()
{
    message *msg;

    msg->size = init_size();
    msg->data = init_data(((msg.size + 3)/4)*4);
    msg->type = init_type(msg->size);
    msg->hash = fnv1_hash(msg->data, strlen(msg->data));

    return msg;
}

int sem_wait(int sem_id, int sem_num) {
    struct sembuf sops = {sem_num, -1, SEM_UNDO};
    return semop(sem_id, &sops, 1);
}

int sem_signal(int sem_id, int sem_num) {
    struct sembuf sops = {sem_num, 1, SEM_UNDO};
    return semop(sem_id, &sops, 1);
}

int main() {
    printf("Hello, World!\n");
    return 0;
}
