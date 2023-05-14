// Лисичкин Д.А. 150502
// бинарное дерево СО СТАНДАРТНЫМ АЛЛОКАТОРОМ
// последнее изменение: 14.05.23

#include "standart_btree.h"

static int st_ml_counter; // счетчик обращений к malloc()
static int st_fr_counter; // счетчик обращений к free()

int st_get_malloc_calls()
{
    return st_ml_counter;
}

int st_get_free_calls()
{
    return st_fr_counter;
}

// Инициализация бинарного дерева
standart_Tree* st_tree_init()
{
    standart_Tree *tree = (standart_Tree*)malloc(sizeof(standart_Tree)); // выделяем память под дерево
    st_ml_counter++;
    tree->root = NULL; // корневой узел пока не существует
    return tree;
}

// Функция создания нового узла дерева
standart_Node* st_create_node(int p_id, char *p_name)
{
    standart_Node* node = (standart_Node*)malloc(sizeof(standart_Node)); // выделяем память под узел
    st_ml_counter++;
    node->p_id = p_id; // сохраняем ключ
    node->p_name = p_name; // сохраняем имя+фамилия
    node->left = NULL; // левый узел пока не существует
    node->right = NULL; // правый узел пока не существует
    return node;
}

// Вставка нового узла в бинарное дерево
void st_insert_node(standart_Tree *tree, int p_id, char *p_name)
{
    // Если дерево пустое, создаем корневой узел
    if (tree->root == NULL)
    {
        tree->root = st_create_node(p_id, p_name);
        return;
    }

    // Начинаем поиск с корневого узла
    struct standart_Node *current = tree->root;

    while (1) // продолжаем поиск пока не найдем место для вставки
    {
        // Если ключ уже существует, обновляем значение и выходим
        if (strcmp(p_name, current->p_name) == 0)
        {
            current->p_id = p_id;
            return;
        }
        else if (strcmp(p_name, current->p_name) < 0) // идем в левое поддерево
        {
            if (current->left == NULL) // если левый узел пустой, создаем новый узел
            {
                current->left = st_create_node(p_id, p_name);
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
                current->right = st_create_node(p_id, p_name);
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
void st_delete_node(standart_Tree *tree, standart_Node *current, char *p_name)
{
    if (current == NULL) return; // если дерево пустое

    // Ищем узел с заданным ключом
    if (strcmp(p_name, current->p_name) < 0)
    {
        st_delete_node(tree, current->left, p_name);
    }
    else if (strcmp(p_name, current->p_name) > 0)
    {
        st_delete_node(tree, current->right, p_name);
    }
    else // если узел найден
    {
        // Узел имеет только одного потомка или является листом
        if (current->left == NULL)
        {
            struct standart_Node *temp = current->right;
            free(current);
            st_fr_counter++;
            current = temp;
        }
        else if (current->right == NULL)
        {
            struct standart_Node *temp = current->left;
            free(current);
            st_fr_counter++;
            current = temp;
        }
        else // Узел имеет двух потомков
        {
            struct standart_Node *temp = st_find_min_node(current->right); // находим минимальный узел в правом поддереве
            current->p_id = temp->p_id; // копируем ключ минимального узла в удаляемый узел
            current->p_name = temp->p_name; // копируем значение минимального узла в удаляемый узел
            st_delete_node(tree, current->right, temp->p_name); // удаляем минимальный узел из правого поддерева
        }
    }
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
void st_delete_key(standart_Tree *tree, char *p_name)
{
    st_delete_node(tree, tree->root, p_name);
}

// Рекурсивная функция для поиска узла в дереве по ключу
standart_Node* st_find_node(standart_Node *root, char *p_name)
{
    if (root == NULL) { // если дерево пустое или узел не найден
        return NULL;
    }

    if (strcmp(p_name, root->p_name) == 0) { // если ключ найден в текущем узле
        return root;
    } else if (strcmp(p_name, root->p_name) < 0) { // если ключ меньше текущего узла, ищем в левом поддереве
        return st_find_node(root->left, p_name);
    } else { // иначе ключ больше текущего узла, ищем в правом поддереве
        return st_find_node(root->right, p_name);
    }
}

// Вывод узлов дерева
void st_print_tree_like_list(standart_Node *node)
{
    if (node == NULL) return;

    st_print_tree_like_list(node->left);
    printf("%02d %s\n", node->p_id, node->p_name);
    st_print_tree_like_list(node->right);
}

// Удаление всего дерева
void st_delete_tree(standart_Tree *tree, standart_Node *node) {
    if (node == NULL) return;

    st_delete_tree(tree, node->left);
    st_delete_tree(tree, node->right);

    free(node);
    st_fr_counter++;
}
