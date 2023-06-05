#ifndef STANDARD_BTREE_H
#define STANDARD_BTREE_H

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "statistic.h"

extern statistic st_stats;

// структура для узла дерева
typedef struct standard_Node
{
    int p_id;                    // идентификатор
    char* p_name;                // имя + фамилия
    struct standard_Node *left;  // указатель на левый узел
    struct standard_Node *right; // указатель на правый узел
} standard_Node;

// структура для бинарного дерева
typedef struct standard_Tree
{
    struct standard_Node* root;     // указатель на корневой узел
} standard_Tree;

// Логгинг
void st_log_action(const char *message);

// Вывод статистики
void st_show_stats();

// Инициализация бинарного дерева
standard_Tree* st_tree_init();

// Функция создания нового узла дерева
standard_Node* st_create_node(int p_id, const char *p_name);

// Вставка нового узла в бинарное дерево
void st_insert_node(standard_Tree *tree, int p_id, const char *p_name);

// Функция поиска минимального ключа в поддереве
standard_Node* st_find_min_node(standard_Node *root);

// Рекурсивная функция для поиска узла в дереве по имени человека
standard_Node* st_find_node_by_p_name(standard_Node *root, const char *p_name);

// Рекурсивная функция для поиска узла в дереве по айди человека
standard_Node* st_find_node_by_id(standard_Node *root, int p_id);

// Рекурсивная функция для поиска РОДИТЕЛЯ узла в дереве по его адресу
standard_Node** st_find_parent_node(standard_Node **current, standard_Node *node);

// Функция для удаления узла из дерева
void st_delete_node(standard_Tree *tree, standard_Node **current, standard_Node **parent);

// Функция для удаления узла с заданным именем
void st_delete_node_by_name(standard_Tree *tree, standard_Node *root, const char *p_name);

// Удаление всего дерева
void st_delete_tree(standard_Tree *tree, standard_Node **node);

// Вывод узлов дерева
void st_print_tree_like_list(standard_Node *node);

// Запись дерева в файл
void st_write_tree_to_file(standard_Node *node, FILE *file);

#endif /* STANDARD_BTREE_H */
