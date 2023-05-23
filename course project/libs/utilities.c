#include "utilities.h"

// Функция чтения строки от пользователя
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

// Функция чтения имени от пользователя
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

// Функция проверки сожержит ли строка только цифры
int is_number(char* str)
{
    int size = strlen(str);
    for (int i = 0; i < size; i++)
    {
        if (str[i] < '0' || str[i] > '9') return 0;
    }
    return 1;
}

// Функция для чистки stdin от лишних символов
void skip_stdin()
{
    char c;
    while ((c = getchar()) != '\n' && c != EOF);
}

// Функция чтения целового числа от пользователя в перделах от l до r
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

// Функция для генерации записей о пользователях
void generate_names(FILE *output_file, FILE *names_file, FILE *surnames_file, int num_persons)
{
    srand(time(NULL));

    if (names_file == NULL || surnames_file == NULL) {
        fprintf(stderr, "Failed to open src/names.txt or src/surnames.txt!\n");
        exit(1);
    }

    int id = 1;
    char name[100];
    char surname[100];

    while (id <= num_persons) {

        int c;

        // Перемещаемся в случайное место в names.txt
        fseek(names_file, 0, SEEK_END);
        long names_file_size = ftell(names_file);
        long names_pos = rand() % names_file_size;
        fseek(names_file, names_pos, SEEK_SET);

        // Ищем начало следующего слова
        while ((c = fgetc(names_file)) != EOF && c != ' ');

        // Читаем имя из names.txt
        if (fscanf(names_file, "%s", name) != 1) {
            continue;
        }

        if (strlen(name) == 0) continue;

        // Перемещаемся в случайное место в surnames.txt
        fseek(surnames_file, 0, SEEK_END);
        long surnames_file_size = ftell(surnames_file);
        long surnames_pos = rand() % surnames_file_size;
        fseek(surnames_file, surnames_pos, SEEK_SET);

        // Ищем начало следующего слова
        while ((c = fgetc(surnames_file)) != EOF && c != ' ');

        // Читаем фамилию из surnames.txt
        if (fscanf(surnames_file, "%s", surname) != 1) {
            continue;
        }

        if (strlen(surname) == 0) continue;

        // Записываем запись в выходной файл
        fprintf(output_file, "%d %s %s\n", id, name, surname);
        id++;
    }
}