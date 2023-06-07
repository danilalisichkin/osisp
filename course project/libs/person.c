#include "person.h"

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

void free_person(struct person *p)
{
    free(p->name);
    free(p);
}

struct person* parse_person(char *line)
{
    char *token = strtok(line, " ");
    if (!is_number(token)) {
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

int add_person(FILE *file, struct person *p)
{
    fseek(file, 0, SEEK_END);
    fprintf(file, "%d %s\n", p->id, p->name);
    return 0;
}

void init_array_of_persons(struct array_of_persons *p_array)
{
    p_array->size = 0;
    p_array->persons = (struct person*)malloc(sizeof(struct person));
}

void free_array_of_persons(struct array_of_persons *p_array)
{
    p_array->size = 0;
    free(p_array->persons);
}

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