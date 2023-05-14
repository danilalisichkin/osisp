// Лисичкин Д.А. 150502
// бинарное дерево
// последнее изменение: 30.04.23

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "my_alloc.h"

// структура для узла дерева
typedef struct Node
{
    int p_id;           // идентификатор
    char *p_name;       // имя + фамилия
    struct Node *left;  // указатель на левый узел
    struct Node *right; // указатель на правый узел
} Node;

// структура для бинарного дерева
typedef struct Tree
{
    struct Node* root;     // указатель на корневой узел
    struct mem_pool* pool; // указатель на пул памяти для выделения узлов
} Tree;
// Инициализация бинарного дерева
Tree* tree_init(struct mem_pool* pool);

// Функция создания нового узла дерева
Node* create_node(struct mem_pool* pool, int p_id, char *p_name);

// Вставка нового узла в бинарное дерево
void insert_node(Tree *tree, int p_id, char *p_name);

// Рекурсивная функция для удаления узла с заданным ключом из дерева
void delete_node(Tree *tree, Node *current, char *p_name);

Node* find_min_node(Node *node);

// Функция для удаления узла с заданным ключом из дерева
void delete_key(Tree *tree, char *p_name);

// Рекурсивная функция для поиска узла в дереве по имени+фамилии
Node* find_node(Node *root, char *p_name);

// Вывод узлов дерева
void print_tree_like_list(Node *node);

// Удаление всего дерева
void delete_tree(Tree *tree, Node *node);

// Запись дерева в файл
void write_tree_to_file(Node* node, FILE *file);