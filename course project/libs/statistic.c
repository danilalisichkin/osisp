#include "statistic.h"

FILE *stats_lwt;
FILE *stats_swt;
FILE *st_stats_lwt;
FILE *st_stats_swt;

void init_stat(statistic *stat)
{
    stat->summary_work_time.tv_nsec = 0;
    stat->summary_work_time.tv_sec = 0;
    stat->last_work_time.tv_nsec = 0;
    stat->last_work_time.tv_sec = 0;
    stat->fl_show_stat = 0;
}

void open_stat_files()
{
    stats_lwt = fopen(STATS_LWT, "w");
    stats_swt = fopen(STATS_SWT, "w");
    st_stats_lwt = fopen(ST_STATS_LWT, "w");
    st_stats_swt = fopen(ST_STATS_SWT, "w");
    if (stats_lwt == NULL || stats_swt == NULL || st_stats_lwt == NULL || st_stats_swt == NULL) {
        fprintf(stderr, "Can't open or create statistic files!\n");
        exit(1);
    }
}

struct timespec get_time_diff(struct timespec time_before, struct timespec time_after)
{
    struct timespec diff;

    if ((time_after.tv_nsec - time_before.tv_nsec) < 0) {
        diff.tv_sec = time_after.tv_sec - time_before.tv_sec - 1;
        diff.tv_nsec = 1000000000 + time_after.tv_nsec - time_before.tv_nsec;
    } else {
        diff.tv_sec = time_after.tv_sec - time_before.tv_sec;
        diff.tv_nsec = time_after.tv_nsec - time_before.tv_nsec;
    }

    return diff;
}

void set_last_work_time(statistic *stat, struct timespec time)
{
    stat->last_work_time.tv_nsec = time.tv_nsec;
    stat->last_work_time.tv_sec = time.tv_sec;
}

void add_to_summary_work_time(statistic *stat, struct timespec time)
{
    stat->summary_work_time.tv_sec += time.tv_sec;

    stat->summary_work_time.tv_nsec += time.tv_nsec;
    if (stat->summary_work_time.tv_nsec >= 1000000000) {
        stat->summary_work_time.tv_nsec -= 1000000000;
        stat->summary_work_time.tv_sec++;
    }
}

char *get_stats(statistic *stat)
{
    if (stat->fl_show_stat) {
        char *buffer = malloc(256 * sizeof(char));
        if (buffer == NULL) {
            fprintf(stderr, "Failed to allocate memory for buffer\n");
            return NULL;
        }

        sprintf(buffer, "LWT: %ld sec. %03lld nsec. | SWT: %ld sec. %lld nsec.",
                stat->last_work_time.tv_sec, (long long int)stat->last_work_time.tv_nsec,
                stat->summary_work_time.tv_sec, (long long int)stat->summary_work_time.tv_nsec);
        return buffer;
    } else {
        return NULL;
    }
}

void allow_show_stat(statistic *stat)
{
    stat->fl_show_stat = 1;
}

void deny_show_stat(statistic *stat)
{
    stat->fl_show_stat = 0;
}
