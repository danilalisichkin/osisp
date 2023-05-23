#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <locale.h>

#include "libs/person.h"
#include "libs/utilities.h"
#include "libs/my_btree.h"
#include "libs/standard_btree.h"

#define NEW_FILE "result.txt"

#define NAME_FILE "src/names.txt"
#define SURNAME_FILE "src/surnames.txt"
#define GENERATED_FILE "src/generated.txt"

void print_launch();

void print_usage();

int read_trees_from_file(FILE *file, Tree *tree, standard_Tree *st_tree);

void edit_person(Tree *tree, standard_Tree *st_tree, struct person _person, int *id);

void find_persons_by_substr(Node *root, const char *p_name, struct array_of_persons *p_array);

struct array_of_persons *find_person(Tree *tree, int id);

int main(int argc, char *argv[]) {

    if (argc < 2 || argc > 3) {
        print_launch();
        exit(1);
    }

    setlocale(LC_ALL, "ru_RU.UTF-8");

    remove(GENERATED_FILE);

    int id;

    FILE *file_was, *file_became;

    struct mem_pool pool;
    struct Tree *tree;
    struct standard_Tree *st_tree;

    size_t file_size;

    if (argc == 2) {
        file_was = fopen(argv[1], "r");
        if (file_was == NULL) {
            fprintf(stderr, "Source \"%s\" file not found!\n", argv[1]);
            exit(1);
        }

        fseek(file_was, 0, SEEK_END);
        size_t file_size = ftell(file_was);
        fseek(file_was, 0, SEEK_SET);

    } else if (argc == 3) {
        if (strcmp(argv[1], "-g") == 0 && is_number(argv[2])) {
            int num = atoi(argv[2]);

            FILE *file_generated = fopen(GENERATED_FILE, "a");
            FILE *file_names = fopen(NAME_FILE, "r");
            FILE *file_surnames = fopen(SURNAME_FILE, "r");

            generate_names(file_generated, file_names, file_surnames, num);

            fseek(file_generated, 0, SEEK_END);
            file_size = ftell(file_generated);

            fclose(file_generated);
            fclose(file_names);

            file_was = fopen(GENERATED_FILE, "r");
        }
        else {
            print_launch();
            exit(1);
        }
    };

    mem_pool_init(&pool, file_size * 20, sizeof(Node));
    tree = tree_init(&pool);
    st_tree = st_tree_init();

    id = read_trees_from_file(file_was, tree, st_tree);

    file_became = fopen(NEW_FILE, "w");

    printf("Enter 'u' to see usage.\n");

    int exit = 0;
    while (!exit) {
        char ch;
        fflush(stdin);
        ch = getchar();
        switch (ch) {
            case '+': {
                skip_stdin();
                char *name = NULL;
                name = read_name();

                insert_node(tree, id, name);
                st_insert_node(st_tree, id, name);
                ++id;

                printf("Person successfully inserted in tree.\n");
                free(name);
                break;
            }
            case '-': {
                struct array_of_persons p_array = *find_person(tree, id);

                if (p_array.size > 0) {
                    printf("'q' - cancel action\n");
                    printf("Choose:\n");

                    char *_ch;
                    while (1) {

                        fflush(stdin);
                        _ch = read_string();

                        if (strcmp(_ch, "q") == 0) {
                            printf("Action canceled.\n");
                            break;
                        }

                        int choice;
                        if (is_number(_ch)) {
                            choice = atoi(_ch);
                        } else continue;

                        if (choice >= 0 && choice < p_array.size) {
                            delete_node_by_name(tree, tree->root, p_array.persons[choice].name);
                            st_delete_node_by_name(st_tree, st_tree->root, p_array.persons[choice].name);
                            printf("Person %02d %s is removed.\n", p_array.persons[choice].id,
                                   p_array.persons[choice].name);
                        } else continue;

                        break;
                    }
                }
                break;
            }
            case 'e': {
                struct array_of_persons p_array = *find_person(tree, id);

                if (p_array.size > 0) {
                    printf("'q' - cancel action\n");
                    printf("Choose:\n");

                    char *_ch;
                    while (1) {

                        fflush(stdin);
                        _ch = read_string();

                        if (strcmp(_ch, "q") == 0) {
                            printf("Action canceled.\n");
                            break;
                        }

                        int choice;
                        if (is_number(_ch)) {
                            choice = atoi(_ch);
                        } else continue;

                        if (choice >= 0 && choice < (int) p_array.size) {
                            edit_person(tree, st_tree, p_array.persons[choice], &id);
                            break;
                        } else continue;
                    }
                }

                break;
            }
            case 'f': {
                find_person(tree, id);
                break;
            }
            case 's': {
                write_tree_to_file(tree->root, file_became);
                printf("Tree successfully saved to file.\n");
                break;
            }
            case 'p': {
                printf("Your tree:\n");
                print_tree_like_list(tree->root);
                break;
            }
            case 'u': {
                print_usage();
                break;
            }
            case 'q': {
                exit = 1;
                break;
            }
        }
    }

    fclose(file_became);
    fclose(file_was);

    remove(GENERATED_FILE);

    st_delete_tree(st_tree, &st_tree->root);
    delete_tree(tree, &tree->root);

    mem_pool_destroy(&pool);

    printf("Count of calling malloc() function: \n");
    printf("my: %d | standard: %d\n", get_malloc_calls(), st_get_malloc_calls());

    printf("Count of calling free() function: \n");
    printf("my: %d | standard: %d\n", get_free_calls(), st_get_free_calls());

    return 0;
}

void print_launch() {
    printf("Error while launching...\n");
    printf("To launch:\n");
    printf("./main -g [NUMBER]\n");
    printf("or\n");
    printf("./main [FILE NAME]\n");
    printf("where\n");
    printf("[NUMBER] - required num of generated persons.\n");
    printf("[FILE NAME] - name of source file with persons.\n");
}

void print_usage() {
    printf("Choose your option:\n");
    printf("'+' - add person.\n");
    printf("'-' - remove person.\n");
    printf("'e' - edit person.\n");
    printf("'f' - find person.\n");
    printf("'s' - save result tree to file.\n");
    printf("'p' - print btree like list.\n");
    printf("'q' - quit.\n");

}

int read_trees_from_file(FILE *file, Tree *tree, standard_Tree *st_tree) {
    int id = -1;

    char *buf = (char *) malloc(256 * sizeof(char));

    while (fgets(buf, 256, file) != NULL && !feof(file)) {
        buf[strcspn(buf, "\n")] = '\0';

        struct person _person;
        _person = *parse_person(buf);

        if (strlen(_person.name) >= MAX_NAME_SIZE) {
            fprintf(stderr, "Can't add person to tree: name is too long.\n");
            continue;
        }

        insert_node(tree, _person.id, _person.name);
        st_insert_node(st_tree, _person.id, _person.name);

        if (_person.id > id) id = _person.id;
    }

    free(buf);
    return id + 1;
}

struct array_of_persons *find_person(Tree *tree, int id) {
    printf("Find person according to:\n");
    printf("'1' - id\n");
    printf("'2' - name\n");
    printf("'q' - cancel action\n");

    char *name = NULL;
    int _id;

    struct array_of_persons *p_array = (struct array_of_persons *) malloc(sizeof(struct array_of_persons));
    init_array_of_persons(p_array);

    char _ch;
    while (1) {
        fflush(stdin);
        _ch = getchar();

        if (_ch == '1') {
            printf("ID: ");
            read_int(&_id, 1, id);

            Node *founded = find_node_by_id(tree->root, _id);

            if (founded != NULL) {
                add_person_to_array(p_array, new_person(founded->p_name, founded->p_id));
            }

            skip_stdin();

        } else if (_ch == '2') {
            skip_stdin();
            name = read_name();
            find_persons_by_substr(tree->root, name, p_array);
        } else if (_ch == 'q') {
            printf("Action canceled.\n");
            return p_array;
        } else {
            continue;
        }
        break;
    }

    printf("Found:\n");
    if (p_array->size > 0) {

        for (int i = 0; i < p_array->size; i++) {
            printf("(%d) %02d %s\n", i, p_array->persons[i].id, p_array->persons[i].name);
        }
    } else {
        printf("Found nothing.\n");
    }

    return p_array;
}

void edit_person(Tree *tree, standard_Tree *st_tree, struct person _person, int *id) {
    printf("Choose option:\n");
    printf("'1' - update id.\n");
    printf("'2' - update name.\n");
    printf("'q' - cancel action.\n");

    while (1) {
        char ch;
        fflush(stdin);
        ch = getchar();

        if (ch == '1') {
            insert_node(tree, _person.id, _person.name);
            st_insert_node(st_tree, _person.id, _person.name);
        } else if (ch == '2') {
            char *new_name = NULL;
            skip_stdin();
            new_name = read_name();

            delete_node_by_name(tree, tree->root, _person.name);
            insert_node(tree, *id, new_name);

            ++(*id);

            free(new_name);
        } else if (ch == 'q') {
            printf("Action canceled.\n");
            break;
        } else continue;

        printf("Person %02d %s is edited.\n", _person.id, _person.name);
        break;
    }
}

void find_persons_by_substr(Node *root, const char *p_name, struct array_of_persons *p_array) {
    if (root == NULL) {
        return;
    }

    find_persons_by_substr(root->left, p_name, p_array);
    if (strstr(root->p_name, p_name) != NULL) {
        add_person_to_array(p_array, new_person(root->p_name, root->p_id));
    }
    find_persons_by_substr(root->right, p_name, p_array);
}
