// Лисичкин Д.А. 150502
// бинарное дерево
// последнее изменение: 30.04.23
// to add:
// 1) надо бы добавить вес к узлам, чтоб потом можно было строить дерево поиска

#include "my_btree.h"

// Инициализация бинарного дерева
Tree* tree_init(struct mem_pool* pool)
{
    Tree* tree = (Tree*)mem_alloc(pool, sizeof(Tree)); // выделяем память под дерево
    tree->root = NULL; // корневой узел пока не существует
    tree->pool = pool; // запоминаем пул памяти для выделения узлов
    return tree;
}

// Функция создания нового узла дерева
Node* create_node(struct mem_pool* pool, int p_id, char *p_name)
{
    Node* node = (Node*)mem_alloc(pool, sizeof(Node)); // выделяем память под узел
    node->p_id = p_id; // сохраняем айди
    node->p_name = p_name; // сохраняем имя+фамилия
    node->left = NULL; // левый узел пока не существует
    node->right = NULL; // правый узел пока не существует
    return node;
}

// Вставка нового узла в бинарное дерево
void insert_node(Tree *tree, int p_id, char *p_name)
{
    // Если дерево пустое, создаем корневой узел
    if (tree->root == NULL)
    {
        tree->root = create_node(tree->pool, p_id, p_name);
        return;
    }

    // Начинаем поиск с корневого узла
    struct Node *current = tree->root;

    while (1) // продолжаем поиск пока не найдем место для вставки
    {
        // Если фамилия+имя уже существует, обновляем значение идентификатора и выходим
        if (strcmp(p_name, current->p_name) == 0)
        {
            current->p_id = p_id;
            return;
        }
        else if (strcmp(p_name, current->p_name) < 0) // идем в левое поддерево
        {
            if (current->left == NULL) // если левый узел пустой, создаем новый узел
            {
                current->left = create_node(tree->pool, p_id, p_name);
                return;
            }
            else // иначе продолжаем поиск в левом поддереве
            {
                current = current->left;
            }
        }
        else // идем в правое поддерево
        {
            if (current->right == NULL) // если правый узел пустой, создаем новый узел
            {
                current->right = create_node(tree->pool, p_id, p_name);
                return;
            }
            else // иначе продолжаем поиск в правом поддереве
            {
                current = current->right;
            }
        }
    }
}

// Рекурсивная функция для удаления узла с заданным ключом из дерева
void delete_node(Tree *tree, Node *current, char *p_name)
{
    if (current == NULL) // если дерево пустое
    {
        return;
    }

    // Ищем узел с заданным ключом
    if (strcmp(p_name, current->p_name) < 0)
    {
        delete_node(tree, current->left, p_name);
    }
    else if (strcmp(p_name, current->p_name) > 0)
    {
        delete_node(tree, current->right, p_name);
    }
    else // если узел найден
    {
        // Узел имеет только одного потомка или является листом
        if (current->left == NULL)
        {
            struct Node *temp = current->right;
            mem_free(tree->pool, current);
            current = temp;
        }
        else if (current->right == NULL)
        {
            struct Node *temp = current->left;
            mem_free(tree->pool, current);
            current = temp;
        }
        else // Узел имеет двух потомков
        {
            struct Node *temp = find_min_node(current->right); // находим минимальный узел в правом поддереве
            current->p_id = temp->p_id; // копируем ключ минимального узла в удаляемый узел
            current->p_name = temp->p_name; // копируем значение минимального узла в удаляемый узел
            delete_node(tree, current->right, temp->p_name); // удаляем минимальный узел из правого поддерева
        }
    }

    // Обновляем высоту узла
    // update_height(current);

    // Балансируем дерево, если необходимо
    // balance_node(tree, current);
}

Node* find_min_node(Node *node) {
    if (node == NULL) {
        return NULL;
    }
    while (node->left != NULL) {
        node = node->left;
    }
    return node;
}

// Функция для удаления узла с заданным ключом из дерева
void delete_key(Tree *tree, char *p_name)
{
    delete_node(tree, tree->root, p_name);
}

// Рекурсивная функция для поиска узла в дереве по ключу
Node* find_node(Node *root, char *p_name)
{
    if (root == NULL) { // если дерево пустое или узел не найден
        return NULL;
    }

    if (strcmp(p_name, root->p_name) == 0) { // если ключ найден в текущем узле
        return root;
    } else if (strcmp(p_name, root->p_name) < 0) { // если ключ меньше текущего узла, ищем в левом поддереве
        return find_node(root->left, p_name);
    } else { // иначе ключ больше текущего узла, ищем в правом поддереве
        return find_node(root->right, p_name);
    }
}

// Удаление всего дерева
void delete_tree(Tree *tree, Node *node) {
    if (node == NULL) {
        return;
    }

    delete_tree(tree, node->left);
    delete_tree(tree, node->right);

    mem_free(tree->pool, node);
}