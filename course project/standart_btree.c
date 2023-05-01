// Лисичкин Д.А. 150502
// бинарное дерево СО СТАНДАРТНЫМ АЛЛОКАТОРОМ
// последнее изменение: 30.04.23
// to add:
// 1) надо бы добавить вес к узлам, чтоб потом можно было строить дерево поиска

#include "standart_btree.h"

static int st_counter; // счетчик обращений к malloc()

int st_get_malloc_calls()
{
    return st_counter;
}

// Инициализация бинарного дерева
standart_Tree* st_tree_init()
{
    standart_Tree *tree = (standart_Tree*)malloc(sizeof(standart_Tree)); // выделяем память под дерево
    st_counter++;
    tree->root = NULL; // корневой узел пока не существует
    return tree;
}

// Функция создания нового узла дерева
standart_Node* st_create_node(int key, char *value)
{
    standart_Node* node = (standart_Node*)malloc(sizeof(standart_Node)); // выделяем память под узел
    st_counter++;
    node->key = key; // сохраняем ключ
    node->value = value; // сохраняем значение
    node->left = NULL; // левый узел пока не существует
    node->right = NULL; // правый узел пока не существует
    return node;
}

// Вставка нового узла в бинарное дерево
void st_insert_node(standart_Tree *tree, int key, char *value)
{
    // Если дерево пустое, создаем корневой узел
    if (tree->root == NULL)
    {
        tree->root = st_create_node(key, value);
        return;
    }

    // Начинаем поиск с корневого узла
    struct standart_Node *current = tree->root;

    while (1) // продолжаем поиск пока не найдем место для вставки
    {
        // Если ключ уже существует, обновляем значение и выходим
        if (current->key == key)
        {
            current->value = value;
            return;
        }
        else if (key < current->key) // идем в левое поддерево
        {
            if (current->left == NULL) // если левый узел пустой, создаем новый узел
            {
                current->left = st_create_node(key, value);
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
                current->right = st_create_node(key, value);
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
void st_delete_node(standart_Tree *tree, standart_Node *current, int key)
{
    if (current == NULL) // если дерево пустое
    {
        return;
    }

    // Ищем узел с заданным ключом
    if (key < current->key)
    {
        st_delete_node(tree, current->left, key);
    }
    else if (key > current->key)
    {
        st_delete_node(tree, current->right, key);
    }
    else // если узел найден
    {
        // Узел имеет только одного потомка или является листом
        if (current->left == NULL)
        {
            struct standart_Node *temp = current->right;
            free(current);
            current = temp;
        }
        else if (current->right == NULL)
        {
            struct standart_Node *temp = current->left;
            free(current);
            current = temp;
        }
        else // Узел имеет двух потомков
        {
            struct standart_Node *temp = st_find_min_node(current->right); // находим минимальный узел в правом поддереве
            current->key = temp->key; // копируем ключ минимального узла в удаляемый узел
            current->value = temp->value; // копируем значение минимального узла в удаляемый узел
            st_delete_node(tree, current->right, temp->key); // удаляем минимальный узел из правого поддерева
        }
    }

    // Обновляем высоту узла
    // update_height(current);

    // Балансируем дерево, если необходимо
    // balance_node(tree, current);
}

standart_Node* st_find_min_node(standart_Node *node) {
    if (node == NULL) {
        return NULL;
    }
    while (node->left != NULL) {
        node = node->left;
    }
    return node;
}

// Функция для удаления узла с заданным ключом из дерева
void st_delete_key(standart_Tree *tree, int key)
{
    st_delete_node(tree, tree->root, key);
}

// Рекурсивная функция для поиска узла в дереве по ключу
standart_Node* st_find_node(standart_Node *root, int key)
{
    if (root == NULL) { // если дерево пустое или узел не найден
        return NULL;
    }

    if (key == root->key) { // если ключ найден в текущем узле
        return root;
    } else if (key < root->key) { // если ключ меньше текущего узла, ищем в левом поддереве
        return st_find_node(root->left, key);
    } else { // иначе ключ больше текущего узла, ищем в правом поддереве
        return st_find_node(root->right, key);
    }
}

// Удаление всего дерева
void st_delete_tree(standart_Tree *tree, standart_Node *node) {
    if (node == NULL) {
        return;
    }

    st_delete_tree(tree, node->left);
    st_delete_tree(tree, node->right);

    free(node);
}