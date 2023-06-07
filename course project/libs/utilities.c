#include "utilities.h"

char* read_string()
{
    char *str = NULL;
    int size = 0;
    int c = EOF;
    int isEmpty = 1;

    while ((c = getchar()) != '\n' && c != EOF) {
        if (c != ' ') isEmpty = 0;
        str = (char*)realloc(str, ++size * sizeof(char));
        str[size-1] = c;
    }

    if (isEmpty && str != NULL) {
        free(str);
        str = NULL;
    }

    if (str != NULL) str[size] = '\0';

    return str;
}

char* read_name()
{
    char *name;
    printf("Name: ");

    while(1)
    {
        name = read_string();
        if (name == NULL || name[0] == '\0') {
            printf("Name can't be empty!\n");
        } else if (strlen(name) >= MAX_NAME_SIZE) {
            printf("Name can't be longer then %d symbols!\n", MAX_NAME_SIZE);
        } else break;
        printf("Repeat: ");
    }

    return name;
}

int is_number(char* str)
{
    int size = strlen(str);
    for (int i = 0; i < size; i++)
    {
        if (str[i] < '0' || str[i] > '9') return 0;
    }
    return 1;
}

void skip_stdin()
{
    char c;
    while ((c = getchar()) != '\n' && c != EOF);
}

void read_int(int *value, int l, int r)
{
    int result;
    while (1)
    {
        result = scanf("%d", value);
        if (!result) {
            printf("Value must be integer!\n");
            skip_stdin();
        } else if (*value < l || *value > r) {
            printf("Value must be from %d to %d!\n", l, r);
        } else break;
        printf("Repeat: ");
    }
}

void generate_names(FILE *output_file, FILE *names_file, FILE *surnames_file, int num_persons)
{
    srand(time(NULL));

    int id = 1;
    char name[100];
    char surname[100];

    while (id <= num_persons) {

        int c;

        fseek(names_file, 0, SEEK_END);
        long names_file_size = ftell(names_file);
        long names_pos = rand() % names_file_size;
        fseek(names_file, names_pos, SEEK_SET);

        while ((c = fgetc(names_file)) != EOF && c != ' ');

        if (fscanf(names_file, "%s", name) != 1) {
            continue;
        }

        if (strlen(name) == 0) continue;

        fseek(surnames_file, 0, SEEK_END);
        long surnames_file_size = ftell(surnames_file);
        long surnames_pos = rand() % surnames_file_size;
        fseek(surnames_file, surnames_pos, SEEK_SET);

        while ((c = fgetc(surnames_file)) != EOF && c != ' ');

        if (fscanf(surnames_file, "%s", surname) != 1) {
            continue;
        }

        if (strlen(surname) == 0) continue;

        fprintf(output_file, "%d %s %s\n", id, name, surname);
        id++;
    }
}

int get_id_from_src_file(FILE *src_file)
{

    long last_pos = ftell(src_file);
    fseek(src_file, 0, SEEK_SET);
    char c;
    long pos = ftell(src_file);
    char *buf = (char *)malloc(256 * sizeof(char));
    int max_id = -1;
    int id = max_id;
    while (fgets(buf, 256, src_file) != NULL && !feof(src_file))
    {
        buf[strcspn(buf, "\n")] = '\0';
        char *token = strtok(buf, " ");
        if (!is_number(token)) {
            fprintf(stderr, "Incorrect structure of source file!\n");
            exit(1);
        }
        id = atoi(token);
        if (id > max_id) max_id = id;
    }
    fseek(src_file, last_pos, SEEK_SET);
    return max_id;
}