#ifndef UTILITIES_H
#define UTILITIES_H

#define MAX_NAME_SIZE 25

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

char* read_string();

char* read_name();

int is_number(char* str);

void skip_stdin();

void read_int(int *value, int l, int r);

void generate_names(FILE *output_file, FILE *names_file, FILE *surnames_file, int num_persons);

int get_id_from_src_file(FILE *src_file);

#endif /* UTILITIES_H */
