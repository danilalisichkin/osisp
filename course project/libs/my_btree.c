#include "my_btree.h"

// Логгинг
void log_action(const char *message)
{
    if (stats.fl_show_stat && message != NULL) {
        printf("%s", message);
    }
}

// Вывод статистики
void show_stats()
{
    static int i;
    fprintf(stats_lwt, "(%d) %ld.%09ld\n",
            i, stats.last_work_time.tv_sec, stats.last_work_time.tv_nsec);
    fprintf(stats_swt, "(%d) %ld.%09ld\n",
            i, stats.summary_work_time.tv_sec, stats.summary_work_time.tv_nsec);
    ++i;
    char *statistic = get_stats(&stats);
    if (statistic != NULL && stats.fl_show_stat) {
        printf("[  MY_ALLOC ] : [%s]\n", statistic);
    }
}

// Инициализация бинарного дерева
Tree* tree_init(struct mem_pool* pool)
{
    struct timespec before, after, diff;

    log_action("[in my_btree] : allocating memory for tree struct...\n");
    clock_gettime(CLOCK_MONOTONIC, &before);
    Tree* tree = (Tree*)mem_alloc(pool, sizeof(Tree)); // выделяем память под дерево
    clock_gettime(CLOCK_MONOTONIC, &after);

    diff = get_time_diff(before, after);
    set_last_work_time(&stats, diff);
    add_to_summary_work_time(&stats, diff);
    show_stats();

    tree->root = NULL; // корневой узел пока не существует
    tree->pool = pool; // запоминаем пул памяти для выделения узлов
    return tree;
}

// Функция создания нового узла дерева
Node* create_node(struct mem_pool* pool, int p_id, const char *p_name)
{
    struct timespec before, after, diff;

    log_action("[in my_btree] : allocating memory for node struct...\n");
    clock_gettime(CLOCK_MONOTONIC, &before);
    Node* node = (Node*)mem_alloc(pool, sizeof(Node)); // выделяем память под узел
    clock_gettime(CLOCK_MONOTONIC, &after);

    diff = get_time_diff(before, after);
    set_last_work_time(&stats, diff);
    add_to_summary_work_time(&stats, diff);
    show_stats();

    node->p_id = p_id; // сохраняем айди
    size_t size = strlen(p_name) + 1;

    log_action("[in my_btree] : allocating memory for person's name...\n");
    clock_gettime(CLOCK_MONOTONIC, &before);
    node->p_name = (char*)mem_alloc(pool, size * sizeof(char)); // сохраняем имя+фамилию
    clock_gettime(CLOCK_MONOTONIC, &after);

    diff = get_time_diff(before, after);
    set_last_work_time(&stats, diff);
    add_to_summary_work_time(&stats, diff);
    show_stats();

    strcpy(node->p_name, p_name);
    node->left = NULL;  // левый узел пока не существует
    node->right = NULL; // правый узел пока не существует
    return node;
}

// Вставка нового узла в бинарное дерево
void insert_node(Tree *tree, int p_id, const char *p_name)
{
    // Если дерево пустое, создаем корневой узел
    if (tree->root == NULL)
    {
        tree->root = create_node(tree->pool, p_id, p_name);
        return;
    }

    // Начинаем поиск с корневого узла
    Node *current = tree->root;

    while (1) // продолжаем поиск пока не найдем место для вставки
    {
        int result = strcmp(p_name, current->p_name);

        // Если фамилия+имя уже существует, обновляем значение идентификатора и выходим
        if (result == 0)
        {
            current->p_id = p_id;
            return;
        }
        else if (result < 0) // идем в левое поддерево
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

// Функция для поиска наименьего узла в дереве
Node* find_min_node(Node *root)
{
    if (root == NULL) {
        return NULL;
    }
    while (root->left != NULL) {
        root = root->left;
    }
    return root;
}

// Рекурсивная функция для поиска узла в дереве по имени человека
Node* find_node_by_p_name(Node *root, const char *p_name)
{
    if (root == NULL) { // если дерево пустое или узел не найден
        return NULL;
    }

    int result = strcmp(p_name, root->p_name);

    if (result == 0) { // если ключ найден в текущем узле
        return root;
    } else if (result < 0) { // если ключ меньше текущего узла, ищем в левом поддереве
        return find_node_by_p_name(root->left, p_name);
    } else { // иначе ключ больше текущего узла
        return find_node_by_p_name(root->right, p_name);
    }
}

// Рекурсивная функция для поиска узла в дереве по айди человека
Node* find_node_by_id(Node *root, int p_id)
{
    if (root == NULL) return NULL;

    if (root->p_id == p_id) return root;

    Node *found = find_node_by_id(root->left, p_id);
    if (found == NULL) {
        found = find_node_by_id(root->right, p_id);
    }
    return found;
}

// Рекурсивная функция для поиска РОДИТЕЛЯ узла в дереве по его адресу
Node** find_parent_node(Node** current, Node* node)
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

    Node** parent = find_parent_node(&((*current)->left), node);
    if (parent != NULL) {
        return parent;
    }

    return find_parent_node(&((*current)->right), node);
}

// Функция для удаления узла с заданным именем
void delete_node_by_name(Tree *tree, Node *root, const char *p_name)
{
    if (root == NULL) return; // нет корневого узла

    Node *to_delete = find_node_by_p_name(tree->root, p_name);
    Node **parent = find_parent_node(&tree->root, to_delete);
    delete_node(tree, &to_delete, parent);
}

// Функция для удаления узла из дерева
void delete_node(Tree *tree, Node **current, Node **parent)
{
    if (*current == NULL) return; // если нет узла

    if (parent == NULL) parent = &(tree->root); // если удаляем корень

    Node *to_delete = *current;

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
        Node *min = find_min_node((*current)->right); // находим минимальный узел в правом поддереве
        Node **parent_min = find_parent_node(current, min); // ищем родителя минимального узла правого поддерева

        if (*current != tree->root) { // Если не корень дерева
            if ((*parent)->right == to_delete) (*parent)->right = min; // Меняем указатель в родительском
            else (*parent)->left = min;
        } else tree->root = min;

        if ((*parent_min)->left == min) (*parent_min)->left = min->right; // Меняем указатель в родит. минимального
        else (*parent_min)->right = min->right;

        min->right = to_delete->right; // Привязывем ветки удаляемого к вставленному вместо него
        min->left = to_delete->left;
    }

    struct timespec before, after, diff;

    log_action("[in my_btree] : freeing memory for person's name...\n");
    clock_gettime(CLOCK_MONOTONIC, &before);
    mem_free(tree->pool, to_delete->p_name); // Очищаем память для удаляемого узла
    clock_gettime(CLOCK_MONOTONIC, &after);

    diff = get_time_diff(before, after);
    set_last_work_time(&stats, diff);
    add_to_summary_work_time(&stats, diff);
    show_stats();

    log_action("[in my_btree] : freeing memory for node struct...\n");
    clock_gettime(CLOCK_MONOTONIC, &before);
    mem_free(tree->pool, to_delete);
    clock_gettime(CLOCK_MONOTONIC, &after);

    diff = get_time_diff(before, after);
    set_last_work_time(&stats, diff);
    add_to_summary_work_time(&stats, diff);
    show_stats();
}

// Удаление всего дерева
void delete_tree(Tree *tree, Node **node)
{
    if (*node == NULL) {
        return;
    }

    delete_tree(tree, &((*node)->left));
    delete_tree(tree, &((*node)->right));

    struct timespec before, after, diff;

    log_action("[in my_btree] : freeing memory for person's name...\n");
    clock_gettime(CLOCK_MONOTONIC, &before);
    mem_free(tree->pool, (*node)->p_name);
    clock_gettime(CLOCK_MONOTONIC, &after);

    diff = get_time_diff(before, after);
    set_last_work_time(&stats, diff);
    add_to_summary_work_time(&stats, diff);
    show_stats();

    log_action("[in my_btree] : freeing memory for node struct...\n");
    clock_gettime(CLOCK_MONOTONIC, &before);
    mem_free(tree->pool, *node);
    clock_gettime(CLOCK_MONOTONIC, &after);

    diff = get_time_diff(before, after);
    set_last_work_time(&stats, diff);
    add_to_summary_work_time(&stats, diff);
    show_stats();

    *node = NULL;
}

// Вывод узлов дерева
void print_tree_like_list(Node *node)
{
    if (node == NULL) return;

    print_tree_like_list(node->left);
    printf("%d %s\n", node->p_id, node->p_name);
    print_tree_like_list(node->right);
}

// Запись дерева в файл
void write_tree_to_file(Node* node, FILE *file)
{
    if (node == NULL) return;

    write_tree_to_file(node->left, file);
    fprintf(file, "%d %s\n", node->p_id, node->p_name);
    write_tree_to_file(node->right, file);
}
