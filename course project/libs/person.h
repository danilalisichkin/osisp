// Лисичкин Д.А. 150502
// класс персона
// класс для иллюстрации работы проекта
// последнее изменение: 19.04.23

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct person {
    char *name;
    int id;
};

// Функция для создания новой структуры person
struct person *new_person(char *name, int id);

// Функция для освобождения памяти, выделенной для структуры person
void free_person(struct person *p);

// Функция для парсинга записи вида "[ID] [NAME]" в структуру person
struct person *parse_person(char *line);

// Функция для добавления записи в текстовый файл
int add_person(FILE *file, struct person *p);