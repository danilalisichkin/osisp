#ifndef UTILITIES_H
#define UTILITIES_H

#define MAX_NAME_SIZE 25

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

// Функция чтения строки от пользователя
char* read_string();

// Функция чтения имени от пользователя
char* read_name();

// Функция проверки сожержит ли строка только цифры
int is_number(char* str);

// Функция для чистки stdin от лишних символов
void skip_stdin();

// Функция чтения целового числа от пользователя в перделах от l до r
void read_int(int *value, int l, int r);

// Функция для генерации записей о пользователях
void generate_names(FILE *output_file, FILE *names_file, FILE *surnames_file, int num_persons);

// Функция для получения айди (кол-ва людей) в файле с записями
int get_id_from_src_file(FILE *src_file);

#endif /* UTILITIES_H */
