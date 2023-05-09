#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include <fcntl.h>

#include <sys/wait.h>
#include <sys/mman.h>
#include <semaphore.h>
#include <time.h>

#include "unistd.h"

#define QUEUE_MAX_SIZE 25
#define MAX_CHILD 10
#define DATA_MAX_SIZE 256

sem_t *mutex, *items, *free_space;

pid_t consumers[MAX_CHILD];
int cons_counter;
pid_t producers[MAX_CHILD];
int prod_counter;

pid_t parent_PID;

typedef struct {
    char type;
    uint16_t hash;
    char size;
    char *data;
} message;

typedef struct {
    message *head;
    int h;
    message *tail;
    int t;
    message buffer[QUEUE_MAX_SIZE];

    int counter_added;
    int counter_extracted;
} queue;

queue *current_queue;

// Алгоритм хэширования FNV-1
uint16_t fnv1_hash(const void *data, size_t size)
{
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
    int real_size = data_size;
    if (data_size == 0) real_size = 256;
    char* new_data = (char*)malloc(real_size*sizeof(char));
    srand(time(NULL));

    for (int i = 0; i < real_size-1; i++) {
        new_data[i] = (rand() % 26) + 'a';
    }
    new_data[real_size] = '\0';

    return new_data;
}

message* create_message()
{
    message *msg;
    msg = (message*)malloc(sizeof (message));

    msg->size = init_size();
    msg->data = init_data(((msg->size + 3)/4)*4);
    msg->type = init_type(msg->size);
    msg->hash = 0;
    msg->hash = fnv1_hash(msg->data, strlen(msg->data));

    return msg;
}

int add_message(message *msg, queue *q)
{

    sem_wait(free_space);
    sem_wait(mutex);

    if (q->counter_added - q->counter_extracted == QUEUE_MAX_SIZE) {
        perror("Can't add more message: overflow!\n");
        exit (1);
    }

    q->tail = msg;
    ++(q->counter_added);
    ++(q->t);
    q->t = q->t % QUEUE_MAX_SIZE;
    q->tail = &q->buffer[q->t];

    sem_post(mutex);
    sem_post(items);

    return q->counter_added;
}

int extract_message(message *msg, queue *q)
{
    sem_wait(items);
    sem_wait(mutex);

    if (q->counter_added == q->counter_extracted) {
        perror("Can't extract more message: queue is empty!\n");
        exit(1);
    }

    msg = q->head;
    ++(q->counter_extracted);
    ++(q->h);
    q->h = q->h % QUEUE_MAX_SIZE;
    q->head = &q->buffer[q->h];

    sem_post(mutex);
    sem_post(free_space);

    return q->counter_extracted;
}

void create_producer()
{
    if (prod_counter == MAX_CHILD) {
        perror("Can't create producer: too much producers!\n");
        return;
    }

    producers[prod_counter] = fork();
    if (producers[prod_counter] == -1) {
        perror("Error creating child process!\n");
        return;
    } else if (producers[prod_counter] > 0) {
        //parent
        ++prod_counter;
        return;
    }
    // child
    message *msg;
    int counter_added;
    while (1) {
        msg = create_message();

        counter_added = add_message(msg, current_queue);

        printf("%d produce msg: hash=%X, counter_added=%d\n",
               getpid(), msg->hash, counter_added);

        sleep(5);
    }
}

void remove_producer()
{
    if (prod_counter == 0) {
        perror("Can't remove producer: no producers!\n");
        return;
    }

    --prod_counter;
    kill(producers[prod_counter], SIGKILL);
    wait(NULL);
}

void create_consumer()
{
    if (cons_counter == MAX_CHILD) {
        perror("Can't create consumer: too much consumers!\n");
        return;
    }

    consumers[cons_counter] = fork();
    if (consumers[cons_counter] == -1) {
        perror("Error creating child process!\n");
        return;
    } else if (consumers[cons_counter] > 0) {
        //parent
        ++cons_counter;
        return;
    }
    // child
    message *msg;
    int counter_extracted;
    while (1) {

        counter_extracted = extract_message(msg, current_queue);

        printf("%d consume msg: hash=%X, counter_extracted=%d\n",
               getpid(), msg->hash, counter_extracted);

        sleep(5);
    }
}

void remove_consumer()
{
    if (cons_counter == 0) {
        perror("Can't remove consumer: no consumers!\n");
        return;
    }

    --cons_counter;
    kill(consumers[cons_counter], SIGKILL);
    wait(NULL);
}

void prepare()
{

    parent_PID = getpid();

    int fd = shm_open("/shared", (O_RDWR | O_CREAT | O_TRUNC), (S_IRUSR | S_IWUSR));
    if (fd < 0) {
        perror("Can't open shared memory!\n");
        exit(1);
    }

    if (ftruncate(fd, sizeof(queue))) {
        perror("Can't initialize memory to share it!\n");
        exit(1);
    }

    void* ptr = mmap(NULL, sizeof(queue), (PROT_READ | PROT_WRITE), MAP_SHARED, fd, 0);
    if (ptr == MAP_FAILED) {
        perror("Can't map shared memory!\n");
        exit(1);
    }

    current_queue = (queue*) ptr;

    if (close(fd)) {
        perror("Can't close file descriptor of '/shared'!\n");
        exit(1);
    }

    current_queue->counter_added = 0;
    current_queue->counter_extracted = 0;
    current_queue->head = NULL;
    current_queue->h = 0;
    current_queue->tail = NULL;
    current_queue->t = 0;
    memset(current_queue->buffer, 0, sizeof(current_queue->buffer));

    mutex = sem_open("mutex", (O_RDWR | O_CREAT | O_TRUNC), (S_IRUSR | S_IWUSR), 1);
    free_space = sem_open("free_space", (O_RDWR | O_CREAT | O_TRUNC), (S_IRUSR | S_IWUSR), QUEUE_MAX_SIZE);
    items = sem_open("items", (O_RDWR | O_CREAT | O_TRUNC), (S_IRUSR | S_IWUSR), 0);
    if (mutex == SEM_FAILED || free_space == SEM_FAILED || items == SEM_FAILED) {
        perror("Can't open semaphore!\n");
        exit(1);
    }
}

void quit()
{
    if (getpid() == parent_PID) {
        for (int i = 0; i < prod_counter; ++i) {
            kill(producers[i], SIGKILL);
            wait(NULL);
        }
        for (int i = 0; i < cons_counter; ++i) {
            kill(consumers[i], SIGKILL);
            wait(NULL);
        }
    } else if (getppid() == parent_PID) {
        kill(getppid(), SIGKILL);
    }

    if (shm_unlink("/shared")) {
        perror("Can't unlink shared memory!\n");
        exit(1);
    }
    if (sem_close(mutex) ||
        sem_close(free_space) ||
        sem_close(items)) {
        perror("Can't close semaphore!\n");
        exit(1);
    }

    exit(0);
}


void test()
{
    printf("PR-S: %d\n", prod_counter);
    printf("CN-S: %d\n", cons_counter);
    printf("AD: %d EX: %d\n", current_queue->counter_added, current_queue->counter_extracted);
}

void print_usage()
{
    printf("Usage: write \n");
    printf("'1' - to create producer.\n");
    printf("'2' - to remove producer.\n");
    printf("'3' - to create consumer.\n");
    printf("'4' - to remove consumer.\n");
    printf("'q' - to quit.\n");
}

int main()
{
    prepare();

    print_usage();

    while(1)
    {
        char ch;
        rewind(stdin);
        ch = getchar();
        switch(ch)
        {
            case '1':
            {
                create_producer();
                break;
            }
            case '2':
            {
                remove_producer();
                break;
            }
            case '3':
            {
                create_consumer();
                break;
            }
            case '4':
            {
                remove_consumer();
                break;
            }
            case 'q':
            {
                quit();
                break;
            }
            case '\n': break;
            case 't':
            {
                test();
                break;
            }
            default:
            {
                print_usage();
                break;
            }
        }
    }
    return 0;
}
