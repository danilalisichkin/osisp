#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include <unistd.h>
#include <signal.h>

#define ALARM_TIME 10000
#define CYCLES 101

struct Stats {
    long pair00;
    long pair01;
    long pair10;
    long pair11;
};

struct Pair {
    int left;
    int right;
};

int flag;
bool isCanOutput;
pid_t parent_PID;
bool endAlarm;

struct sigaction signal_SIGUSR1, signal_alarm;

void alarm_handler(int signal);
void sigusr1_handler(int signal);
void set_handles();
void get_stat(struct Stats *stats, struct Pair *pair);
void reset_stat(struct Stats *stats);

int main(int argc, char *argv[]) {
    if (argc < 1) {
        perror("Error in calling child program!\n");
        exit(1);
    }
    parent_PID = getppid();
    isCanOutput = false;
    set_handles();
    printf("IN %s process, PID %d : program is launched\n", argv[0], getpid());
    struct Stats stats = {0, 0, 0, 0};
    flag = 4;
    while(1) {
        for (int i = 0; i < CYCLES; i++) {
            struct Pair pair;
            endAlarm = false;
            ualarm(ALARM_TIME, 0);
            while (!endAlarm) {
                flag += (flag == 0 ? 4 : 0);
                pair.right = (flag % 2 == 0 ? 0 : 1);
                pair.left = (flag > 2 ? 0 : 1);
                flag--;
            }
            get_stat(&stats, &pair);
        }
        char buf[128];
        sprintf(buf, "%s, %d: 00:%ld, 01:%ld, 10:%ld, 11:%ld", argv[0], getpid(), stats.pair00, stats.pair01, stats.pair10, stats.pair11);
        kill(parent_PID, SIGUSR1);
        if (isCanOutput) {
            isCanOutput = false;
            printf("%s\n", buf);
        }
        reset_stat(&stats);
    }
    return 0;
}

void alarm_handler(int signal)
{
    endAlarm = true;
}

void sigusr1_handler(int signal)
{
    isCanOutput = true;
}

void set_handles()
{
    signal_SIGUSR1.sa_handler = sigusr1_handler;
    sigaction(SIGUSR1, &signal_SIGUSR1, NULL);

    signal_alarm.sa_handler = alarm_handler;
    sigaction(SIGALRM, &signal_alarm, NULL);
}

void get_stat(struct Stats *stats, struct Pair *pair)
{
    if (!pair->left && !pair->right){
        stats->pair00++;
    } else if (!pair->left && pair->right) {
        stats->pair01++;
    } else if (pair->left && !pair->right) {
        stats->pair10++;
    } else {
        stats->pair11++;
    }
}

void reset_stat(struct Stats *stats)
{
    stats->pair00 = 0;
    stats->pair01 = 0;
    stats->pair10 = 0;
    stats->pair11 = 0;
}