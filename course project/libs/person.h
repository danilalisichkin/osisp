#ifndef PERSON_H
#define PERSON_H

#define MAX_NAME_SIZE 25

#include <unistd.h>
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

struct person *new_person(char *name, int id);

void free_person(struct person *p);

struct person *parse_person(char *line);

int add_person(FILE *file, struct person *p);

void init_array_of_persons(struct array_of_persons *p_array);

void free_array_of_persons(struct array_of_persons *p_array);

void add_person_to_array(struct array_of_persons *p_array, struct person* _person);

#endif /* PERSON_H */
