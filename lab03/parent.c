#define _GNU_SOURCE
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>
#include <time.h>
#include <locale.h>

#define MAX_CHILD 10
#define MAX_STRING_LEN 10
int CUR_CHILD = -1;

struct processes
{
    char name[MAX_STRING_LEN];
    pid_t PID;
    bool isCanShowStat;
} child_PROC[MAX_CHILD];

pid_t parent_PID;
struct sigaction signal_SIGUSR1;
int last_used_child;

struct timespec sleeptime;

void create_new_child_name(char* name, int counter);
void print_usage();
int get_choice();
int find_by_pid(pid_t pid);
void sigusr1_handler(int sig, siginfo_t *info, void *context);
void set_signals();
void call_child(int index);
void add_child();
void remove_last_child();
void show_processes_list();
void remove_all_childs();
void allow_show_stats_all();
void deny_show_stats_all();

int main()
{
    sleeptime.tv_sec = 5;
    sleeptime.tv_nsec = 5000000000;
    last_used_child = -1;
    parent_PID = getpid();
    set_signals();
    printf("Enter 'u' to see usage.\n");
    printf("Parent PID %d\n", parent_PID);
    while(1)
    {
        rewind(stdin);
        switch(getchar())
        {
            case '+':
            {
                add_child();
                break;
            }
            case '-':
            {
                remove_last_child();
                printf("%d child processes left.\n", CUR_CHILD+1);
                break;
            }
            case 'l':
            {
                show_processes_list();
                break;
            }
            case 'k':
            {
                remove_all_childs();
                printf("%d child processes left.\n", CUR_CHILD+1);
                break;
            }
            case 's':
            {
                int choice = get_choice();
                if (choice > -1) {
                    last_used_child = choice;
                    child_PROC[last_used_child].isCanShowStat = false;
                } else if (choice == -2) {
                    deny_show_stats_all();
                }
                break;
            }
            case 'g':
            {
                int choice = get_choice();
                if (choice > -1) {
                    last_used_child = choice;
                    child_PROC[last_used_child].isCanShowStat = true;
                } else if (choice == -2) {
                    allow_show_stats_all();
                }
                break;
            }
            case 'p':
            {
                int choice = get_choice();
                if (choice > -1) {
                    last_used_child = choice;
                    deny_show_stats_all();
                    sleep(2);
                    printf("Waiting for child_%02d...\n", choice);
                    child_PROC[last_used_child].isCanShowStat = true;
                    sleep(5);
                    allow_show_stats_all();
                }
                break;
            }
            case 'q':
            {
                remove_all_childs();
                printf("Parent process PID %d has stopped.\n", parent_PID);
                return 0;
            }
            case 'u':
            {
                print_usage();
                break;
            }
            default:
            {
                break;
            }
        }
    }
}

void create_new_child_name(char* name, int counter)
{
    if (counter >= 0 && counter < 100) {
        sprintf(name, "child_%d%d", counter/10, counter%10);
    }
    else {
        perror("Can't create name!\n");
    }
}

void print_usage()
{
    printf("Usage:\n");
    printf("Write '+'      to launch new process\n");
    printf("      '-'      to remove last process\n");
    printf("      'l'      to show all launched processes\n");
    printf("      'k'      to remove all processes\n");
    printf("      's'      to deny showing stats from all processes\n");
    printf("    's<num>'   to deny showing stats from <num> process\n");
    printf("      'g'      to allow showing stats from all processes\n");
    printf("    'g<num>'   to allow showing stats from <num> process\n");
    printf("    'p<num>'   to show stats from <num> process\n");
    printf("      'q'      to end program\n");
}

int get_choice()
{
    int choice;
    char ch2;
    rewind(stdin);
    ch2 = getchar();
    if (ch2 == '\n') {
        choice = -2;
    } else if (ch2 < '0' || ch2 > '9') {
        choice = -1;
    } else {
        choice = ch2 - '0';
    }
    if (choice > CUR_CHILD) {
        printf("There is no such process!\n");
        choice = -1;
    }
    return choice;
}

int find_by_pid(pid_t pid)
{
    int i;
    for (i = 0; child_PROC[i].PID != pid; i++);
    return i;
}

void sigusr1_handler(int sig, siginfo_t *info, void *context)
{
    int i = find_by_pid(info->si_pid);
    if (child_PROC[i].isCanShowStat) {
        kill(child_PROC[i].PID, SIGUSR1);
    }
}

void set_signals()
{
    signal_SIGUSR1.sa_flags =  SA_SIGINFO;
    signal_SIGUSR1.sa_sigaction = sigusr1_handler;
    if (sigaction(SIGUSR1, &signal_SIGUSR1, NULL)) {
        perror("Sigaction error!\n");
    }
}

void call_child(int index)
{
    char name[MAX_STRING_LEN];
    create_new_child_name(name, CUR_CHILD+1);
    if (execlp("./child", name, NULL) == -1) {
        perror("Child launching failed\n");
    }
}

void add_child()
{
    if (CUR_CHILD + 1 >= MAX_CHILD) {
        printf("Child launching failed: too much child processes are working\n");
        return;
    }
    pid_t pid = fork();

    if (pid < 0) {
        perror("Fork failed\n");
        exit(0);
    } else if (pid > 0) {
        CUR_CHILD++;
        child_PROC[CUR_CHILD].PID = pid;
        create_new_child_name(child_PROC[CUR_CHILD].name, CUR_CHILD);
        printf("%s process, PID %d was created.\n", child_PROC[CUR_CHILD].name, child_PROC[CUR_CHILD].PID);
    } else {
        call_child(last_used_child);
    }
}

void remove_last_child()
{
    if (CUR_CHILD + 1 <= 0) {
        printf("Child removing failed: no child processes launched\n");
    } else {
        kill(child_PROC[CUR_CHILD].PID, SIGTERM);
        printf("%s process, PID %d was removed.\n", child_PROC[CUR_CHILD].name, child_PROC[CUR_CHILD].PID);
        CUR_CHILD--;
    }
}

void show_processes_list()
{
    printf("Parent process PID %d\n", getpid());
    if (CUR_CHILD + 1 <= 0) {
        printf("No child processes launched.\n");
    } else {
        printf("Child processes launched:\n");
        for (int i = 0; i <= CUR_CHILD; i++)
        {
            printf("%s process, PID %d is launched. [%d]\n", child_PROC[i].name, child_PROC[i].PID, (child_PROC[i].isCanShowStat ? 1:0));
        }
    }
}

void remove_all_childs()
{
    while (CUR_CHILD >= 0)
    {
        remove_last_child();
    }
}

void allow_show_stats_all()
{
    for(int i=0; i <= CUR_CHILD; i++)
    {
        child_PROC[i].isCanShowStat = true;
    }
}

void deny_show_stats_all()
{
    for(int i=0; i <= CUR_CHILD; i++)
    {
        child_PROC[i].isCanShowStat = false;
    }
}
