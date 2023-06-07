#define __USE_POSIX199506

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <locale.h>

#include "libs/person.h"
#include "libs/utilities.h"
#include "libs/my_btree.h"
#include "libs/standard_btree.h"

#define NAME_FILE "src/names.txt"
#define SURNAME_FILE "src/surnames.txt"
#define GENERATED_FILE "src/generated.txt"

int num_of_persons;

void print_launch();

void print_usage();

int read_trees_from_file(FILE *file, Tree *tree, standard_Tree *st_tree);

void edit_person(Tree *tree, standard_Tree *st_tree, struct person _person, int *id);

void find_persons_by_substr(Node *root, const char *p_name, struct array_of_persons *p_array);

struct array_of_persons *find_person(Tree *tree, int id);

void remove_random_persons(Tree* tree, standard_Tree *st_tree, int number, int max_id);

int main(int argc, char *argv[])
{

    if (argc < 2 || argc > 3) {
        print_launch();
        exit(1);
    }

    setlocale(LC_ALL, "ru_RU.UTF-8");

    remove(GENERATED_FILE);
    remove(STATS_LWT);
    remove(STATS_SWT);
    remove(ST_STATS_LWT);
    remove(ST_STATS_SWT);

    num_of_persons = 0;
    int id;

    FILE *file_was = NULL;

    struct mem_pool pool;
    struct Tree *tree;
    struct standard_Tree *st_tree;

    if (argc == 2) {
        file_was = fopen(argv[1], "r");
        if (file_was == NULL) {
            fprintf(stderr, "Source \"%s\" file not found!\n", argv[1]);
            exit(1);
        }
        id = get_id_from_src_file(file_was);
    } else if (argc == 3) {
        if (strcmp(argv[1], "-g") == 0 && is_number(argv[2])) {
            int num = atoi(argv[2]);

            FILE *file_generated = fopen(GENERATED_FILE, "a");
            FILE *file_names = fopen(NAME_FILE, "r");
            FILE *file_surnames = fopen(SURNAME_FILE, "r");

            if (file_names == NULL || file_surnames == NULL) {
                fprintf(stderr, "Failed to open src/names.txt or src/surnames.txt!\n");
                exit(1);
            }

            generate_names(file_generated, file_names, file_surnames, num);

            fclose(file_generated);
            fclose(file_names);

            file_was = fopen(GENERATED_FILE, "r");
            id = num;
        }
        else {
            print_launch();
            exit(1);
        }
    };

    size_t pool_size = id * (sizeof(Node) + MAX_NAME_SIZE);
    if (id <= 10) {
        pool_size += 5 * pool_size;
    } else if (id > 10 && id <= 50) {
        pool_size += 2 * pool_size;
    } else if (id > 50 && id <= 100) {
        pool_size += pool_size;
    } else if (id > 100 && id <= 250) {
        pool_size += pool_size;
    } else if (id > 250 && id <= 500) {
        pool_size += pool_size;
    } else if (id > 500) {
        pool_size += pool_size / 2;
    }

    char *choose_strategy = "Choose allocation strategy.\n"
                            "'1' - FIRST_FIT.\n"
                            "'2' - BEST_FIT.\n"
                            "'3' - WORST_FIT.\n";
    printf("%s", choose_strategy);
    char mode;
    while (1)
    {
        fflush(stdin);
        mode = getchar();
        if (mode == '1') mem_pool_init(&pool, pool_size, FIRST_FIT);
        else if (mode == '2') mem_pool_init(&pool, pool_size, BEST_FIT);
        else if (mode == '3') mem_pool_init(&pool, pool_size, WORST_FIT);
        else continue;
        break;
    }

    init_stat(&stats);
    init_stat(&st_stats);

    open_stat_files();

    char *see_stats = "Want see statistic?\n"
                      "'y' - yes.\n"
                      "'n' - no.\n";
    printf("%s", see_stats);
    while (1)
    {
        fflush(stdin);
        mode = getchar();
        if (mode == 'y') {
            allow_show_stat(&stats);
            allow_show_stat(&st_stats);
            break;
        }
        else if (mode == 'n') break;
        else continue;
    }

    tree = tree_init(&pool);
    st_tree = st_tree_init();

    fseek(file_was, 0, SEEK_SET);
    id = read_trees_from_file(file_was, tree, st_tree);

    printf("Enter 'u' to see usage.\n");

    int exit = 0;
    while (!exit)
    {
        char ch;
        fflush(stdin);
        ch = getchar();
        switch (ch) {
            case 'a': {
                allow_show_stat(&stats);
                allow_show_stat(&st_stats);
                printf("Allowed showing stats.\n");
                break;
            }
            case 'd': {
                deny_show_stat(&stats);
                deny_show_stat(&st_stats);
                printf("Denied showing stats.\n");
                break;
            }
            case '!': {
                printf("[ POOL STAT ] : [%s]\n", get_pool_stats(&pool));
                break;
            }
            case '+': {
                skip_stdin();
                char *name = NULL;
                name = read_name();

                insert_node(tree, id, name);
                st_insert_node(st_tree, id, name);
                ++id;
                ++num_of_persons;
                printf("(+) %d %s successfully inserted in tree.\n", id-1, name);
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
                            --num_of_persons;
                            printf("(-) %d %s is removed.\n", p_array.persons[choice].id,
                                   p_array.persons[choice].name);
                        } else continue;

                        break;
                    }
                }
                break;
            }
            case 'r': {
                if (num_of_persons < 10) {
                    fprintf(stderr, "Can't remove 10 persons: theres only %d persons.\n", num_of_persons);
                    break;
                }
                remove_random_persons(tree, st_tree, 10, id);
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
                printf("Write file’s name: ");
                char *f_name;
                skip_stdin();
                f_name = read_string();
                FILE* file_result = NULL;
                file_result = fopen(f_name, "w");
                if (file_result == NULL) {
                    fprintf(stderr, "Can't create result file!\n");
                    break;
                }
                write_tree_to_file(tree->root, file_result);
                printf("Tree successfully saved to file.\n");
                fclose(file_result);
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

    fclose(file_was);

    remove(GENERATED_FILE);

    char *pool_stats = get_pool_stats(&pool);

    delete_tree(tree, &tree->root);
    st_delete_tree(st_tree, &st_tree->root);

    printf("Result statistic:\n");
    printf("[ POOL STAT ] : [%s]\n", pool_stats);

    mem_pool_destroy(&pool);

    allow_show_stat(&stats);
    show_stats();
    allow_show_stat(&st_stats);
    st_show_stats();

    free(pool_stats);
    return 0;
}

void print_launch()
{
    char* launch = "Error while launching...\n"
                   "To launch:\n"
                   "./main -g [NUMBER]\n"
                   "or\n"
                   "./main [FILE NAME]\n"
                   "where\n"
                   "[NUMBER] - required num of generated persons.\n"
                   "[FILE NAME] - name of source file with persons.\n";
    printf("%s", launch);
}

void print_usage()
{
    char* menu = "Choose your option:\n"
                 "'a' - allow showing statistic.\n"
                 "'d' - deny showing statistic.\n"
                 "'!' - show pool statistic.\n"
                 "'+' - add person.\n"
                 "'-' - remove person.\n"
                 "'r' - remove 10 random person.\n"
                 "'e' - edit person.\n"
                 "'f' - find person.\n"
                 "'s' - save result tree to file.\n"
                 "'p' - print btree like list.\n"
                 "'q' - quit.\n";
    printf("%s", menu);
}

int read_trees_from_file(FILE *file, Tree *tree, standard_Tree *st_tree)
{
    int id = -1;

    char *buf = (char *)malloc(256 * sizeof(char));

    while (fgets(buf, 256, file) != NULL && !feof(file))
    {
        buf[strcspn(buf, "\n")] = '\0';

        struct person _person;
        _person = *parse_person(buf);

        if (strlen(_person.name) >= MAX_NAME_SIZE) {
            fprintf(stderr, "Can't add [%d %s] to tree: name is too long.\n",
                    _person.id, _person.name);
            continue;
        }

        insert_node(tree, _person.id, _person.name);
        st_insert_node(st_tree, _person.id, _person.name);

        ++num_of_persons;

        if (_person.id > id) id = _person.id;
    }

    free(buf);
    return id + 1;
}

struct array_of_persons *find_person(Tree *tree, int id)
{
    printf("Find person according to:\n");
    printf("'1' - id\n");
    printf("'2' - name\n");
    printf("'q' - cancel action\n");

    char *name = NULL;
    int _id;

    struct array_of_persons *p_array = (struct array_of_persons *)malloc(sizeof(struct array_of_persons));
    init_array_of_persons(p_array);

    char _ch;
    while (1)
    {
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
            printf("(%d) %d %s\n", i, p_array->persons[i].id, p_array->persons[i].name);
        }
    } else {
        printf("Found nothing.\n");
    }

    return p_array;
}

void edit_person(Tree *tree, standard_Tree *st_tree, struct person _person, int *id)
{
    printf("Choose option:\n");
    printf("'1' - update id.\n");
    printf("'2' - update name.\n");
    printf("'q' - cancel action.\n");

    while (1)
    {
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

            st_delete_node_by_name(st_tree, st_tree->root, _person.name);
            st_insert_node(st_tree, *id, new_name);

            ++(*id);

            free(new_name);
        } else if (ch == 'q') {
            printf("Action canceled.\n");
            break;
        } else continue;

        printf("(e) %d %s is edited.\n", _person.id, _person.name);
        break;
    }
}

void remove_random_persons(Tree* tree, standard_Tree *st_tree, int number, int max_id)
{
    int _id;
    for (int i = 0; i < number; ++i)
    {
        _id = rand() % max_id;

        Node *to_delete = find_node_by_id(tree->root, _id);

        if (to_delete == NULL) {
            --i;
            continue;
        } else {
            printf("(-) %d %s is removed.\n", to_delete->p_id, to_delete->p_name);
            delete_node(tree, &to_delete, find_parent_node(&tree->root, to_delete));
        }

        standard_Node *st_to_delete = st_find_node_by_id(st_tree->root, _id);
        if (st_to_delete == NULL) {
            --i;
            continue;
        } else {
            st_delete_node(st_tree, &st_to_delete, st_find_parent_node(&st_tree->root, st_to_delete));
        }

        --num_of_persons;
    }
}

void find_persons_by_substr(Node *root, const char *p_name, struct array_of_persons *p_array)
{
    if (root == NULL) {
        return;
    }

    find_persons_by_substr(root->left, p_name, p_array);
    if (strstr(root->p_name, p_name) != NULL) {
        add_person_to_array(p_array, new_person(root->p_name, root->p_id));
    }
    find_persons_by_substr(root->right, p_name, p_array);
}
