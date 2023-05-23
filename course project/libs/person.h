#ifndef PERSON_H
#define PERSON_H

// Лисичкин Д.А. 150502
// класс персона
// класс для иллюстрации работы проекта
// последнее изменение: 19.04.23

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "utilities.h"

struct person {
    char *name;
    int id;
};

struct array_of_persons
{
    int size;
    struct person *persons;
};

// Функция для создания новой структуры person
struct person *new_person(char *name, int id);

// Функция для освобождения памяти, выделенной для структуры person
void free_person(struct person *p);

// Функция для парсинга записи вида "[ID] [NAME]" в структуру person
struct person *parse_person(char *line);

// Функция для добавления записи в текстовый файл
int add_person(FILE *file, struct person *p);

// Функция для инициализации массива людей
void init_array_of_persons(struct array_of_persons *p_array);

// Функция для очистки массива людей
void free_array_of_persons(struct array_of_persons *p_array);

// Функция для добавления человека в массив людей
void add_person_to_array(struct array_of_persons *p_array, struct person* _person);

#endif /* PERSON_H */
