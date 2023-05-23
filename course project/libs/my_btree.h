#ifndef MY_BTREE_H
#define MY_BTREE_H

// Лисичкин Д.А. 150502
// бинарное дерево
// последнее изменение: 14.05.23

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
    struct Node *root; // указатель на корневой узел
    struct mem_pool *pool; // указатель на пул памяти для выделения узлов
} Tree;

// Инициализация бинарного дерева
Tree* tree_init(struct mem_pool *pool);

// Функция создания нового узла дерева
Node* create_node(struct mem_pool *pool, int p_id, const char *p_name);

// Вставка нового узла в бинарное дерево
void insert_node(Tree *tree, int p_id, const char *p_name);

// Функция для поиска наименьшего узла в дереве
Node* find_min_node(Node *root);

// Рекурсивная функция для поиска узла в дереве по имени человека
Node* find_node_by_p_name(Node *root, const char *p_name);

// Рекурсивная функция для поиска узла в дереве по айди человека
Node* find_node_by_id(Node *root, int p_id);

// Рекурсивная функция для поиска РОДИТЕЛЯ узла в дереве по его адресу
Node** find_parent_node(Node **current, Node *node);

// Функция для удаления узла из дерева
void delete_node(Tree *tree, Node **current, Node **parent);

// Функция для удаления узла с заданным именем
void delete_node_by_name(Tree *tree, Node *root, const char *p_name);

// Удаление всего дерева
void delete_tree(Tree *tree, Node **node);

// Вывод узлов дерева
void print_tree_like_list(Node *node);

// Запись дерева в файл
void write_tree_to_file(Node *node, FILE *file);

#endif /* MY_BTREE_H */
