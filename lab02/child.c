#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>

int main(int argc, char* argv[])
{
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <filename>\n", argv[0]);
        return 1;
    }

    printf("%s pid: %d ppid: %d \n", argv[0], getpid(), getppid()); 

    FILE *filepath;
    filepath = fopen(argv[1], "r");
    if (filepath == NULL) {
        fprintf(stderr, "Error opening file %s\n", argv[1]);
        return 1;
    }

    char* key = (char*)calloc(255, sizeof(char));
    char* key_tmp = (char*)calloc(255, sizeof(char));

    int count = 0;

    while(!feof(filepath))
    {
        if (fscanf(filepath, "%s", key) != 1) {
            fprintf(stderr, "Error reading from file %s\n", argv[1]);
            return 1;
        }
        if(!strcmp(key, key_tmp)) {
            break;
        }
        char* value = getenv(key);
        if (value == NULL) {
            printf("%s = NOT SET!\n", key); 
        } else {
            printf("%s = %s\n", key, value);
        }
        strcpy(key_tmp, key);
        count++;
    }

    fclose(filepath);

    if (count == 0) {
        printf("No environment variables found in file %s\n", argv[1]);
    }

    return 0;
}