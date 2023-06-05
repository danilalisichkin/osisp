#include "person.h"

// Функция для создания новой структуры person
struct person* new_person(char *name, int id)
{
    struct person *_person = (struct person*)malloc(sizeof(struct person));
    int size = strlen(name) + 1;
    _person->name = (char*)malloc(size * sizeof(char));
    strcpy(_person->name, name);
    _person->name[size-1] = '\0';
    _person->id = id;
    return _person;
}

// Функция для освобождения памяти, выделенной для структуры person
void free_person(struct person *p)
{
    free(p->name);
    free(p);
}

// Функция для парсинга записи вида "[ID] [NAME]" в структуру person
struct person* parse_person(char *line)
{
    char *token = strtok(line, " ");
    if (!is_number(token)) { // Если структура файла нарушена
        fprintf(stderr, "Incorrect structure of source file!\n");
        exit(1);
    }
    int id = atoi(token);
    token = strtok(NULL, "\n");
    int size = strlen(token) + 1;
    if (size >= MAX_NAME_SIZE) {
        fprintf(stderr, "Person's name is longer then %d symbols.\n", MAX_NAME_SIZE);
        exit(1);
    }
    char *name = (char*)malloc(size * sizeof(char));
    strcpy(name, token);
    name[size-1] = '\0';
    struct person *_peson = new_person(name, id);
    free(name);
    return _peson;
}

// Функция для добавления записи в текстовый файл
int add_person(FILE *file, struct person *p)
{
    fseek(file, 0, SEEK_END);
    fprintf(file, "%d %s\n", p->id, p->name);
    return 0;
}

// Функция для инициализации массива людей
void init_array_of_persons(struct array_of_persons *p_array)
{
    p_array->size = 0;
    p_array->persons = (struct person*)malloc(sizeof(struct person));
}

// Функция для очистки массива людей
void free_array_of_persons(struct array_of_persons *p_array)
{
    p_array->size = 0;
    free(p_array->persons);
}

// Функция для добавления человека в массив людей
void add_person_to_array(struct array_of_persons *p_array, struct person* _person)
{
    if (p_array->size == 0) {
        ++(p_array->size);
        p_array->persons = (struct person*)malloc(sizeof(struct person));
    } else {
        ++(p_array->size);
        p_array->persons = (struct person*)realloc(p_array->persons, (p_array->size)*sizeof(struct person));
    }
    p_array->persons[p_array->size - 1] = *_person;
}