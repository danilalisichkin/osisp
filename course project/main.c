#include "stdbool.h"

#include "libs/person.h"
#include "libs/my_btree.h"
#include "libs/standart_btree.h"

#define OLD_FILE "was.txt"
#define NEW_FILE "became.txt"

void write_tree_to_file(Node *node, FILE *file);

int find_by_name(Node *root, char *p_name, struct person *persons, int *size);

char* read_string();

void skip_stdin();

int main()
{
    int id;

    FILE *file_was, *file_became;
    file_was = fopen(OLD_FILE, "r");
    file_became = fopen(NEW_FILE, "a");

    struct mem_pool pool;
    mem_pool_init(&pool, 1024 * 1024);

    struct Tree *tree;
    tree = tree_init(&pool);

    struct standart_Tree *st_tree;
    st_tree = st_tree_init();

    int p_index = 0;

    char *buf = malloc(256 * sizeof(char));

    while (fgets(buf, 256, file_was))
    {
        buf[strcspn(buf, "\n")] = '\0';
        struct person _person;
        _person = *parse_person(buf);
        insert_node(tree, _person.id, _person.name);
        st_insert_node(st_tree, _person.id, _person.name);
        id = _person.id;
    }

    free(buf);

    printf("[COUNTER OF CALLING MALLOC() FUNCTION]\n");
    printf("IN MY TREE: %d | IN STANDARD TREE: %d\n", get_malloc_calls(), st_get_malloc_calls());

    printf("Choose your option:\n");
    printf("'1' - search by name/surname in tree.\n");
    printf("'2' - sort by name+surname to file.\n");
    printf("'3' - add person to a btree\n");
    printf("'4' - show btree like list\n");
    printf("'q' - exit.\n");

    bool exit = false;
    while (!exit)
    {
        char ch;
        rewind(stdin);
        ch = getchar();
        switch(ch)
        {
            case '1':
            {
                skip_stdin();
                char *name;
                printf("Name: ");
                
                while(1)
                {
                    name = read_string();
                    if (name != NULL) break;
                    printf("Name can't be empty!\n Repeat: ");
                }
                

                struct person *persons = (struct person*)malloc(sizeof(struct person));
                int size = 0;

                printf("\nYour result:\n");

                if (find_by_name(tree->root, name, persons, &size)) {
                    for (int i = 0; i < size; i++)
                    {
                        printf("%02d %s\n", persons[i].id, persons[i].name);
                    }
                } else {
                    printf("Found nothing.\n");
                }
                break;
            }
            case '2':
            {
                write_tree_to_file(tree->root, file_became);
                printf("Tree successfully writen to file.\n");
                break;
            }
            case '3':
            {
            	skip_stdin();
                char *name = NULL;
                printf("Name: ");

                while(1)
                {
                    name = read_string();
                    if (name != NULL) break;
                    printf("Name can't be empty!\n Repeat: ");
                }

                insert_node(tree, ++id, name);
                printf("Person successfully inserted in tree.\n");
                break;
            }
            case '4':
            {
                printf("Your tree:\n");
                print_tree_like_list(tree->root);
                break;
            }
            case 'q':
            {
                exit = true;
                break;
            }
        }
    }

    delete_tree(tree, tree->root);
    st_delete_tree(st_tree, st_tree->root);

    mem_pool_destroy(&pool);

    fclose(file_was);
    fclose(file_became);

    printf("[COUNTER OF CALLING FREE() FUNCTION]\n");
    printf("IN MY TREE: %d | IN STANDARD TREE: %d\n", get_free_calls(), st_get_free_calls());

    return 0;
}

int find_by_name(Node *root, char *p_name, struct person *persons, int *size)
{
    if (root == NULL) {
        return *size;
    }

    *size = find_by_name(root->left, p_name, persons, size);
    if (strstr(root->p_name, p_name) != NULL) {
        persons[(*size)++] = *new_person(root->p_name, root->p_id);
    }
    *size = find_by_name(root->right, p_name, persons, size);
    return *size;
}

char* read_string() {
    char *str = NULL;
    int size = 0;
    int c = EOF;
    bool isEmpty = true;

    while ((c = getchar()) != '\n' && c != EOF) {
        if (c != ' ') isEmpty = false;
        str = realloc(str, ++size);
        str[size-1] = c;
    }

    if (isEmpty && str != NULL) {
        free(str);
        str = NULL;
    }

    if (str != NULL) str[size] = '\0';

    return str;
}

void skip_stdin()
{
    char c;
    do
    {
        c = getchar();
    } while (c!='\n' && c!=EOF);
}
