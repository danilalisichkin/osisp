#include "my_btree.h"

void log_action(const char *message)
{
    if (stats.fl_show_stat && message != NULL) {
        printf("%s", message);
    }
}

void show_stats()
{
    fprintf(stats_lwt, "%ld s %ld nsec\n",
            stats.last_work_time.tv_sec, stats.last_work_time.tv_nsec);
    fprintf(stats_swt, "%ld s %ld nsec\n",
            stats.summary_work_time.tv_sec, stats.summary_work_time.tv_nsec);
    char *statistic = get_stats(&stats);
    if (statistic != NULL && stats.fl_show_stat) {
        printf("[  MY_ALLOC ] : [%s]\n", statistic);
    }
}

Tree* tree_init(struct mem_pool* pool)
{
    struct timespec before, after, diff;

    log_action("[in my_btree] : allocating memory for tree struct...\n");
    clock_gettime(CLOCK_MONOTONIC, &before);
    Tree* tree = (Tree*)mem_alloc(pool, sizeof(Tree));
    clock_gettime(CLOCK_MONOTONIC, &after);

    diff = get_time_diff(before, after);
    set_last_work_time(&stats, diff);
    add_to_summary_work_time(&stats, diff);
    show_stats();

    tree->root = NULL; 
    tree->pool = pool; 
    return tree;
}

Node* create_node(struct mem_pool* pool, int p_id, const char *p_name)
{
    struct timespec before, after, diff;

    log_action("[in my_btree] : allocating memory for node struct...\n");
    clock_gettime(CLOCK_MONOTONIC, &before);
    Node* node = (Node*)mem_alloc(pool, sizeof(Node)); 
    clock_gettime(CLOCK_MONOTONIC, &after);

    diff = get_time_diff(before, after);
    set_last_work_time(&stats, diff);
    add_to_summary_work_time(&stats, diff);
    show_stats();

    node->p_id = p_id; 
    size_t size = strlen(p_name) + 1;

    log_action("[in my_btree] : allocating memory for person's name...\n");
    clock_gettime(CLOCK_MONOTONIC, &before);
    node->p_name = (char*)mem_alloc(pool, size * sizeof(char)); 
    clock_gettime(CLOCK_MONOTONIC, &after);

    diff = get_time_diff(before, after);
    set_last_work_time(&stats, diff);
    add_to_summary_work_time(&stats, diff);
    show_stats();

    strcpy(node->p_name, p_name);
    node->left = NULL;  
    node->right = NULL; 
    return node;
}

void insert_node(Tree *tree, int p_id, const char *p_name)
{
    if (tree->root == NULL)
    {
        tree->root = create_node(tree->pool, p_id, p_name);
        return;
    }

    Node *current = tree->root;

    while (1)
    {
        int result = strcmp(p_name, current->p_name);

        if (result == 0)
        {
            current->p_id = p_id;
            return;
        }
        else if (result < 0)
        {
            if (current->left == NULL)
            {
                current->left = create_node(tree->pool, p_id, p_name);
                return;
            }
            else
            {
                current = current->left;
            }
        }
        else
        {
            if (current->right == NULL)
            {
                current->right = create_node(tree->pool, p_id, p_name);
                return;
            }
            else
            {
                current = current->right;
            }
        }
    }
}

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

Node* find_node_by_p_name(Node *root, const char *p_name)
{
    if (root == NULL) {
        return NULL;
    }

    int result = strcmp(p_name, root->p_name);

    if (result == 0) {
        return root;
    } else if (result < 0) {
        return find_node_by_p_name(root->left, p_name);
    } else {
        return find_node_by_p_name(root->right, p_name);
    }
}

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

Node** find_parent_node(Node** current, Node* node)
{
    if (*current == NULL || node == NULL) {
        return NULL;
    }

    if (*current == node) {
        return NULL;
    }

    if ((*current)->left == node || (*current)->right == node) {
        return current;
    }

    Node** parent = find_parent_node(&((*current)->left), node);
    if (parent != NULL) {
        return parent;
    }

    return find_parent_node(&((*current)->right), node);
}

void delete_node_by_name(Tree *tree, Node *root, const char *p_name)
{
    if (root == NULL) return;

    Node *to_delete = find_node_by_p_name(tree->root, p_name);
    Node **parent = find_parent_node(&tree->root, to_delete);
    delete_node(tree, &to_delete, parent);
}

void delete_node(Tree *tree, Node **current, Node **parent)
{
    if (*current == NULL) return;

    if (parent == NULL) parent = &(tree->root);

    Node *to_delete = *current;

    if ((*current)->left == NULL && (*current)->right == NULL) {
        if ((*parent)->right == to_delete) (*parent)->right = NULL; 
        else (*parent)->left = NULL;
    }
    else if ((*current)->left == NULL) {
        if ((*parent)->right == to_delete) (*parent)->right = (*current)->right;
        else (*parent)->left = (*current)->right;
    }
    else if ((*current)->right == NULL)
    {
        if ((*parent)->right == to_delete) (*parent)->right = (*current)->left;
        else (*parent)->left = (*current)->left;
    }
    else
    {
        Node *min = find_min_node((*current)->right); 
        Node **parent_min = find_parent_node(current, min);

        if (*current != tree->root) {
            if ((*parent)->right == to_delete) (*parent)->right = min; 
            else (*parent)->left = min;
        } else tree->root = min;

        if ((*parent_min)->left == min) (*parent_min)->left = min->right; 
        else (*parent_min)->right = min->right;

        min->right = to_delete->right; 
        min->left = to_delete->left;
    }

    struct timespec before, after, diff;

    log_action("[in my_btree] : freeing memory for person's name...\n");
    clock_gettime(CLOCK_MONOTONIC, &before);
    mem_free(tree->pool, to_delete->p_name); 
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

void print_tree_like_list(Node *node)
{
    if (node == NULL) return;

    print_tree_like_list(node->left);
    printf("%d %s\n", node->p_id, node->p_name);
    print_tree_like_list(node->right);
}

void write_tree_to_file(Node* node, FILE *file)
{
    if (node == NULL) return;

    write_tree_to_file(node->left, file);
    fprintf(file, "%d %s\n", node->p_id, node->p_name);
    write_tree_to_file(node->right, file);
}
