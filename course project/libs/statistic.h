#ifndef STATISTIC_H
#define STATISTIC_H

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

extern FILE *stats_lwt;
#define STATS_LWT "statistic/my_alloc/lwt.txt"
extern FILE *stats_swt;
#define STATS_SWT "statistic/my_alloc/swt.txt"
extern FILE *st_stats_lwt;
#define ST_STATS_LWT "statistic/malloc/lwt.txt"
extern FILE *st_stats_swt;
#define ST_STATS_SWT "statistic/malloc/swt.txt"

typedef struct statistic
{
    struct timespec summary_work_time;  
    struct timespec last_work_time;     
    int fl_show_stat;                   
} statistic;

void init_stat(statistic *stat);

void open_stat_files();

struct timespec get_time_diff(struct timespec time_before, struct timespec time_after);

void set_last_work_time(statistic *stat, struct timespec time);

void add_to_summary_work_time(statistic *stat, struct timespec time);

char *get_stats(statistic *stat);

void allow_show_stat(statistic *stat);

void deny_show_stat(statistic *stat);

#endif //STATISTIC_H