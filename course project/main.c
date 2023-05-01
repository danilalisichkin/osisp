// В программе открываются 2 файла:
// 1ый - для чтения, 2ой - для записи
// считываем из 1го файла список людей отсортированных по id
// заносим список людей в бинарные деревья 2ух типов:
// 1ое - с моим аллокатором, 2ое - со стандартным аллокатором
// добавляем узлы для последующего поиска --> считаем кол-во обращений к malloc().
// результат в консоль: кол-во обращений при разном алгоритме аллокации,
// делаем выводы, какой лучше работает :)

// 1) при выборе опции "Поиск по фамилии / имени":
// ищем в дереве записи, возвращаем массив подходящих людей.
// 2) при выборе опции "Сортировка в файл":
// проходимся по дереву в отсортированном порядке по имени+фамилии,
// записываем во 2ой файл.

#include "stdbool.h"

#include "person.h"
#include "my_btree.h"
#include "standart_btree.h"

#define OLD_FILE "was.txt"
#define NEW_FILE "became.txt"

void write_tree_to_file(Node *node, FILE *file);
struct person* find_by_name(Node *root, char *p_name);

int main() {
    // ПОДГОТОВКА
    FILE *file_was, *file_became;
    file_was = fopen(OLD_FILE, "r");
    file_became = fopen(NEW_FILE, "a");

    struct mem_pool pool;
    mem_pool_init(&pool, 1024 * 1024);

    struct Tree *tree;
    tree = tree_init(&pool);

    struct standart_Tree *standart_tree;
    standart_tree = st_tree_init();

    struct person *persons;
    persons = (struct person *) malloc(sizeof(struct person));
    int p_index = 0;

    char *buf = malloc(256 * sizeof(char));

    while (fgets(buf, 256, file_was))
    {
        buf[strcspn(buf, "\n")] = '\0'; // удаление символа перевода строки из считанной строки
        persons = (struct person *) realloc(persons, (++p_index) * sizeof(struct person));
        persons[p_index - 1] = *parse_person(buf);
    }

    free(buf);

    for (int i = 0; i < p_index; i++)
    {
        insert_node(tree, persons[i].id, persons[i].name);
        st_insert_node(standart_tree, persons[i].id, persons[i].name);
    }

    // пока чисто для теста

    printf("\n\n\n[RESULT FROM MY TREE]:\n\n");
    struct Node* node = tree->root;
    while (node != NULL)
    {
        printf("%02d %s\n", node->p_id, node->p_name);
        node=node->right;
    }
    printf("[MY COUNTER]: %d\n", get_malloc_calls());

    printf("\n\n\n[RESULT FROM STANDART TREE]:\n\n");
    struct standart_Node * st_node = standart_tree->root;
    while (st_node != NULL)
    {
        printf("%02d %s\n", st_node->key, st_node->value);
        st_node=st_node->right;
    }
    printf("[STANDART COUNTER]: %d\n", st_get_malloc_calls());

    printf("Choose your option:\n");
    printf("(1) - search by name/surname in tree.\n");
    printf("(2) - sort by name+surname to file.\n");
    printf("(Q) - exit.\n");

    bool exit = false;
    while(exit == false)
    {
        char ch;
        rewind(stdin);
        ch = getchar();
        switch(ch)
        {
            case '1': // ОШИБКА СЕГМЕНТАЦИИ
            {
                char* name;
                printf("Name: ");
                scanf("%s", name);
                struct person* persons = find_by_name(tree->root, name);
                printf("\nYour result:\n");
                int i = 0;
                while (persons[i].name!=NULL)
                {
                    printf("%02d %s", persons[i].id, persons[i].name);
                    i++;
                }
                break;
            }
            case '2':
            {
                write_tree_to_file(tree->root, file_became);
            }
            case 'q':
            {
                exit = true;
            }
        }
    }

    free(persons);
    //mem_pool_destroy(&pool);
    fclose(file_was);
    fclose(file_became);

    return 0;
}

void write_tree_to_file(Node* node, FILE *file)
{
    if (node == NULL) return;
    write_tree_to_file(node->left, file);
    fprintf(file, "%02d %s\n", node->p_id, node->p_name);
    write_tree_to_file(node->right, file);
}


struct person* find_by_name(Node *root, char *p_name)
{
    static struct person* persons;
    static int size;

    if (root == NULL) { // если дерево пустое или узел не найден
        return NULL;
    }
    find_by_name(root->left, p_name);
    if (strstr(root->p_name, p_name) != NULL) {
        persons = (struct person*) realloc(persons, (++size) * sizeof(struct person));
        persons[size-1] = *new_person(root->p_name, root->p_id);
    }
    find_by_name(root->right, p_name);
    return persons;
}