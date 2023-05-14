// Лисичкин Д.А. 150502
// бинарное дерево СО СТАНДАРТНЫМ АЛЛОКАТОРОМ
// последнее изменение: 14.05.23

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// структура для узла дерева
typedef struct standart_Node
{
    int p_id;            // ключ
    char* p_name;          // значение
    struct standart_Node *left;  // указатель на левый узел
    struct standart_Node *right; // указатель на правый узел
} standart_Node;

// структура для бинарного дерева
typedef struct standart_Tree
{
    struct standart_Node* root;     // указатель на корневой узел
} standart_Tree;

int st_get_malloc_calls();

int st_get_free_calls();

// Инициализация бинарного дерева
standart_Tree* st_tree_init();
// Функция создания нового узла дерева
standart_Node* st_create_node(int p_id, char *p_name);

// Вставка нового узла в бинарное дерево
void st_insert_node(standart_Tree *tree, int p_id, char *p_name);

// Рекурсивная функция для удаления узла с заданным ключом из дерева
void st_delete_node(standart_Tree *tree, struct standart_Node *current, char *p_name);

// Функция для удаления узла с заданным ключом из дерева
void st_delete_key(standart_Tree *tree, char *p_name);

// Функция поиска минимального ключа в поддереве
standart_Node* st_find_min_node(standart_Node *node);

// Рекурсивная функция для поиска узла в дереве по ключу
standart_Node* st_find_node(standart_Node* root, char *p_name);

// Вывод узлов дерева
void st_print_tree_like_list(standart_Node *node);

// Удаление всего дерева
void st_delete_tree(standart_Tree *tree, standart_Node* node);
