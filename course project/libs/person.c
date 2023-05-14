// Лисичкин Д.А. 150502
// класс персона
// класс для иллюстрации работы проекта
// последнее изменение: 19.04.23

#include "person.h"

// Функция для создания новой структуры person
struct person* new_person(char *name, int id) {
    struct person *p = malloc(sizeof(struct person));
    p->name = malloc(sizeof(name));
    strcpy(p->name, name);
    p->id = id;
    return p;
}

// Функция для освобождения памяти, выделенной для структуры person
void free_person(struct person *p) {
    free(p->name);
    free(p);
}

// Функция для парсинга записи вида "[ID] [NAME]" в структуру person
struct person* parse_person(char *line) {
    char *token = strtok(line, " ");
    int id = atoi(token);
    token = strtok(NULL, "\n");
    char *name = malloc(sizeof(token));
    strcpy(name, token);
    return new_person(name, id);
}

// Функция для добавления записи в текстовый файл
int add_person(FILE *file, struct person *p) {
    fseek(file, 0, SEEK_END);
    fprintf(file, "%d %s\n", p->id, p->name);
    return 0;
}