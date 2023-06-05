#include "standard_btree.h"

statistic st_stats;

// Логгинг
void st_log_action(const char *message)
{
    if (st_stats.fl_show_stat && message != NULL) {
        printf("%s", message);
    }
}

// Вывод статистики
void st_show_stats()
{
    static int i;
    fprintf(st_stats_lwt, "(%d) %ld.%09ld\n",
            i, st_stats.last_work_time.tv_sec, st_stats.last_work_time.tv_nsec);
    fprintf(st_stats_swt, "(%d) %ld.%09ld\n",
            i, st_stats.summary_work_time.tv_sec, st_stats.summary_work_time.tv_nsec);
    ++i;
    char *statistic = get_stats(&st_stats);
    if (statistic != NULL) {
        printf("[   MALLOC  ] : [%s]\n", statistic);
        free(statistic);
    }
}

// Инициализация бинарного дерева
standard_Tree* st_tree_init()
{
    struct timespec before, after, diff;

    st_log_action("[in st_btree] : allocating memory for tree struct...\n");
    clock_gettime(CLOCK_MONOTONIC, &before);
    standard_Tree *tree = (standard_Tree*)malloc(sizeof(standard_Tree)); // выделяем память под дерево
    clock_gettime(CLOCK_MONOTONIC, &after);

    diff = get_time_diff(before, after);
    set_last_work_time(&st_stats, diff);
    add_to_summary_work_time(&st_stats, diff);
    st_show_stats();

    tree->root = NULL; // корневой узел пока не существует
    return tree;
}

// Функция создания нового узла дерева
standard_Node* st_create_node(int p_id, const char *p_name)
{
    struct timespec before, after, diff;

    st_log_action("[in st_btree] : allocating memory for node struct...\n");
    clock_gettime(CLOCK_MONOTONIC, &before);
    standard_Node* node = (standard_Node*)malloc(sizeof(standard_Node)); // выделяем память под узел
    clock_gettime(CLOCK_MONOTONIC, &after);

    diff = get_time_diff(before, after);
    set_last_work_time(&st_stats, diff);
    add_to_summary_work_time(&st_stats, diff);
    st_show_stats();

    node->p_id = p_id; // сохраняем айди

    st_log_action("[in st_btree] : allocating memory for person's name...\n");
    clock_gettime(CLOCK_MONOTONIC, &before);
    node->p_name = malloc(strlen(p_name) + 1);
    clock_gettime(CLOCK_MONOTONIC, &after);

    diff = get_time_diff(before, after);
    set_last_work_time(&st_stats, diff);
    add_to_summary_work_time(&st_stats, diff);
    st_show_stats();

    strcpy(node->p_name, p_name); // сохраняем имя+фамилию
    node->left = NULL; // левый узел пока не существует
    node->right = NULL; // правый узел пока не существует
    return node;
}

// Вставка нового узла в бинарное дерево
void st_insert_node(standard_Tree *tree, int p_id, const char *p_name)
{
    // Если дерево пустое, создаем корневой узел
    if (tree->root == NULL)
    {
        tree->root = st_create_node(p_id, p_name);
        return;
    }

    // Начинаем поиск с корневого узла
    standard_Node *current = tree->root;

    while (1) // продолжаем поиск пока не найдем место для вставки
    {

        int result = strcmp(p_name, current->p_name);

        // Если ключ уже существует, обновляем значение и выходим
        if (result == 0)
        {
            current->p_id = p_id;
            return;
        }
        else if (result < 0) // идем в левое поддерево
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

// Функция поиска минимального узла
standard_Node* st_find_min_node(standard_Node *root) {
    if (root == NULL) {
        return NULL;
    }
    while (root->left != NULL) {
        root = root->left;
    }
    return root;
}

// Рекурсивная функция для поиска узла в дереве по имени человека
standard_Node* st_find_node_by_p_name(standard_Node *root, const char *p_name)
{
    if (root == NULL) { // если дерево пустое или узел не найден
        return NULL;
    }

    int result = strcmp(p_name, root->p_name);

    if (result == 0) { // если ключ найден в текущем узле
        return root;
    } else if (result < 0) { // если ключ меньше текущего узла, ищем в левом поддереве
        return st_find_node_by_p_name(root->left, p_name);
    } else { // иначе ключ больше текущего узла
        return st_find_node_by_p_name(root->right, p_name);
    }
}

// Рекурсивная функция для поиска узла в дереве по айди человека
standard_Node* st_find_node_by_id(standard_Node *root, int p_id)
{
    if (root == NULL) return NULL;

    if (root->p_id == p_id) return root;

    standard_Node *found = st_find_node_by_id(root->left, p_id);
    if (found == NULL) {
        found = st_find_node_by_id(root->right, p_id);
    }
    return found;
}

// Рекурсивная функция для поиска РОДИТЕЛЯ узла в дереве по его адресу
standard_Node** st_find_parent_node(standard_Node** current, standard_Node* node)
{
    if (*current == NULL || node == NULL) {
        return NULL;
    }

    // Если искомый узел является корневым узлом, то он не имеет родителя
    if (*current == node) {
        return NULL;
    }

    if ((*current)->left == node || (*current)->right == node) {
        return current; // Нашли родителя
    }

    standard_Node** parent = st_find_parent_node(&((*current)->left), node);
    if (parent != NULL) {
        return parent;
    }

    return st_find_parent_node(&((*current)->right), node);
}

// Функция для удаления узла с заданным именем
void st_delete_node_by_name(standard_Tree *tree, standard_Node *root, const char *p_name)
{
    if (root == NULL) return; // нет корневого узла

    standard_Node *to_delete = st_find_node_by_p_name(tree->root, p_name);
    standard_Node **parent = st_find_parent_node(&tree->root, to_delete);
    st_delete_node(tree, &to_delete, parent);
}

// Функция для удаления узла из дерева
void st_delete_node(standard_Tree *tree, standard_Node **current, standard_Node **parent)
{
    if (*current == NULL) return; // если нет узла

    if (parent == NULL) parent = &tree->root;

    standard_Node *to_delete = *current;

    if ((*current)->left == NULL && (*current)->right == NULL) { // Если нет потомков
        if ((*parent)->right == to_delete) (*parent)->right = NULL; // Зануляем указатель на удаляемый в родительском
        else (*parent)->left = NULL;
    }
    else if ((*current)->left == NULL) { // Если есть только правый узел
        if ((*parent)->right == to_delete) (*parent)->right = (*current)->right; // Меняем указатель в родительском
        else (*parent)->left = (*current)->right;
    }
    else if ((*current)->right == NULL) // Если есть только левый узел
    {
        if ((*parent)->right == to_delete) (*parent)->right = (*current)->left; // Меняем указатель в родительском
        else (*parent)->left = (*current)->left;
    }
    else // Узел имеет двух потомков
    {
        standard_Node *min = st_find_min_node((*current)->right); // находим минимальный узел в правом поддереве
        standard_Node **parent_min = st_find_parent_node(current, min); // ищем родителя минимального узла правого поддерева

        if (*current != tree->root) { // Если не корень дерева
            if ((*parent)->right == to_delete) (*parent)->right = min; // Меняем указатель в родительском
            else (*parent)->left = min;
        } else tree->root = min;

        if ((*parent_min)->left == min) (*parent_min)->left = min->right;   // Меняем указатель в родит. минимального
        else (*parent_min)->right = min->right;

        min->right = to_delete->right; // Привязывем ветки удаляемого к вставленному вместо него
        min->left = to_delete->left;
    }

    struct timespec before, after, diff;

    st_log_action("[in st_btree] : freeing memory for person's name...\n");
    clock_gettime(CLOCK_MONOTONIC, &before);
    free(to_delete->p_name); // Очищаем память для удаляемого узла
    clock_gettime(CLOCK_MONOTONIC, &after);

    diff = get_time_diff(before, after);
    set_last_work_time(&st_stats, diff);
    add_to_summary_work_time(&st_stats, diff);
    st_show_stats();

    st_log_action("[in st_btree] : freeing memory for node struct...\n");
    clock_gettime(CLOCK_MONOTONIC, &before);
    free(to_delete);
    clock_gettime(CLOCK_MONOTONIC, &after);

    diff = get_time_diff(before, after);
    set_last_work_time(&st_stats, diff);
    add_to_summary_work_time(&st_stats, diff);
    st_show_stats();
}

// Удаление всего дерева
void st_delete_tree(standard_Tree *tree, standard_Node **node)
{
    if (*node == NULL) {
        return;
    }

    st_delete_tree(tree, &((*node)->left));
    st_delete_tree(tree, &((*node)->right));

    struct timespec before, after, diff;

    st_log_action("[in st_btree] : freeing memory for person's name...\n");
    clock_gettime(CLOCK_MONOTONIC, &before);
    free((*node)->p_name);
    clock_gettime(CLOCK_MONOTONIC, &after);

    diff = get_time_diff(before, after);
    set_last_work_time(&st_stats, diff);
    add_to_summary_work_time(&st_stats, diff);
    st_show_stats();

    st_log_action("[in st_btree] : freeing memory for node struct...\n");
    clock_gettime(CLOCK_MONOTONIC, &before);
    free(*node);
    clock_gettime(CLOCK_MONOTONIC, &after);

    diff = get_time_diff(before, after);
    set_last_work_time(&st_stats, diff);
    add_to_summary_work_time(&st_stats, diff);
    st_show_stats();

    *node = NULL;
}

// Вывод узлов дерева
void st_print_tree_like_list(standard_Node *node)
{
    if (node == NULL) return;

    st_print_tree_like_list(node->left);
    printf("%02d %s\n", node->p_id, node->p_name);
    st_print_tree_like_list(node->right);
}

// Запись дерева в файл
void st_write_tree_to_file(standard_Node *node, FILE *file)
{
    if (node == NULL) return;

    st_write_tree_to_file(node->left, file);
    fprintf(file, "%02d %s\n", node->p_id, node->p_name);
    st_write_tree_to_file(node->right, file);
}
