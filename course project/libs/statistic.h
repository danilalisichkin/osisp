#ifndef STATISTIC_H
#define STATISTIC_H

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

// Файлы статистики и пути к ним
extern FILE *stats_lwt;
#define STATS_LWT "statistic/my_alloc/lwt.txt"
extern FILE *stats_swt;
#define STATS_SWT "statistic/my_alloc/swt.txt"
extern FILE *st_stats_lwt;
#define ST_STATS_LWT "statistic/malloc/lwt.txt"
extern FILE *st_stats_swt;
#define ST_STATS_SWT "statistic/malloc/swt.txt"

// Статистика для сравнения аллокации
typedef struct statistic
{
    struct timespec summary_work_time;  // суммарное время работы аллокаторов
    struct timespec last_work_time;     // время работы последней операции
    int fl_show_stat;                   // флаг разрешения на вывод статистики
} statistic;

// Инициализация статистики
void init_stat(statistic *stat);

// Открытие файлов со статистикой
void open_stat_files();

// Вычисление времени выполнения кода
struct timespec get_time_diff(struct timespec time_before, struct timespec time_after);

// Установка времени работы последней операции
void set_last_work_time(statistic *stat, struct timespec time);

// Добавление времени работы
void add_to_summary_work_time(statistic *stat, struct timespec time);

// Вывод статистики
char *get_stats(statistic *stat);

// Разрешение вывода статистики
void allow_show_stat(statistic *stat);

// Запрещение вывода статистики
void deny_show_stat(statistic *stat);

#endif //STATISTIC_H