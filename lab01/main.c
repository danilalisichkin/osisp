#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

#define MAX_PATH 1024

int getopt(int argc, char * const argv[], const char *optstring);
int lstat(const char *file_name, struct stat *buf);

void print_tutorial();

void dirwalk(const char *path);

struct show_params
{
    bool show_dirs;
    bool show_files;
    bool show_links;
    bool do_sort;
} param;

int main(int argc, char *argv[]) {

    char *dir_path = ".";
    int opt;
    extern int optind;
    while ((opt = getopt(argc, argv, "ldfs")) != -1) {
        switch (opt) 
        {
        case 'd':
            param.show_dirs = true;
            break;
        case 'f':
            param.show_files = true;
            break;
        case 'l':
            param.show_links = true;
            break;
        case 's':
            param.do_sort = true;
            break;
        default:
            print_tutorial();
            exit(EXIT_FAILURE);
        }
    }

    if (param.show_dirs == false && param.show_files == false && param.show_links == false){
        param.show_dirs = true;
        param.show_files = true;
        param.show_links = true;
    }

    if (optind < argc) {
        dir_path = argv[optind];
    }

    dirwalk(dir_path);
    return 0;
}


void print_tutorial()
{
    printf("To use: dirwalk [params] [directory]\n");
    printf("Params: -f -d -l -s\n");
    printf("-f - show files\n");
    printf("-d - show directories\n");
    printf("-l - show symbolic links\n");
    printf("-s - sort according to LC_COLLATE\n");
}

void dirwalk(const char *path) {
    DIR *dir;
    struct dirent *entry;
    struct stat info;
    char subdir[MAX_PATH];

    if ((dir = opendir(path)) == NULL) {
        fprintf(stderr, "Error opening directory %s: %s\n", path, strerror(errno));
        return;
    }

    while ((entry = readdir(dir)) != NULL) {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
            continue;
        }

        snprintf(subdir, MAX_PATH, "%s/%s", path, entry->d_name);

        if (lstat(subdir, &info) < 0) {
            fprintf(stderr, "Error getting information for file %s: %s\n", subdir, strerror(errno));
            continue;
        }

        if (S_ISDIR(info.st_mode)) {
            if (param.show_dirs) {
                printf("%s\n", subdir);
            }
            dirwalk(subdir);
            continue;
        } else if ((S_ISREG(info.st_mode) && param.show_files) || (S_ISLNK(info.st_mode) && param.show_links)){
                printf("%s\n", subdir);
            }
    }
    closedir(dir);
    return;
}
