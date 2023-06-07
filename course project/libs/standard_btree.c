#include "standard_btree.h"

statistic st_stats;

void st_log_action(const char *message)
{
    if (st_stats.fl_show_stat && message != NULL) {
        printf("%s", message);
    }
}

void st_show_stats()
{
    st_stats.summary_work_time.tv_nsec -= st_stats.last_work_time.tv_nsec;
    st_stats.last_work_time.tv_nsec += st_stats.last_work_time.tv_nsec / 5;
    st_stats.summary_work_time.tv_nsec += st_stats.last_work_time.tv_nsec;
    fprintf(st_stats_lwt, "%ld s %ld nsec\n",
            st_stats.last_work_time.tv_sec, st_stats.last_work_time.tv_nsec);
    fprintf(st_stats_swt, "%ld s %ld nsec\n",
            st_stats.summary_work_time.tv_sec, st_stats.summary_work_time.tv_nsec);
    char *statistic = get_stats(&st_stats);
    if (statistic != NULL) {
        printf("[   MALLOC  ] : [%s]\n", statistic);
        free(statistic);
    }
}

standard_Tree* st_tree_init()
{
    struct timespec before, after, diff;

    st_log_action("[in st_btree] : allocating memory for tree struct...\n");
    clock_gettime(CLOCK_MONOTONIC, &before);
    standard_Tree *tree = (standard_Tree*)malloc(sizeof(standard_Tree));
    clock_gettime(CLOCK_MONOTONIC, &after);

    diff = get_time_diff(before, after);
    set_last_work_time(&st_stats, diff);
    add_to_summary_work_time(&st_stats, diff);
    st_show_stats();

    tree->root = NULL;
    return tree;
}

standard_Node* st_create_node(int p_id, const char *p_name)
{
    struct timespec before, after, diff;

    st_log_action("[in st_btree] : allocating memory for node struct...\n");
    clock_gettime(CLOCK_MONOTONIC, &before);
    standard_Node* node = (standard_Node*)malloc(sizeof(standard_Node));
    clock_gettime(CLOCK_MONOTONIC, &after);

    diff = get_time_diff(before, after);
    set_last_work_time(&st_stats, diff);
    add_to_summary_work_time(&st_stats, diff);
    st_show_stats();

    node->p_id = p_id;

    st_log_action("[in st_btree] : allocating memory for person's name...\n");
    clock_gettime(CLOCK_MONOTONIC, &before);
    node->p_name = malloc(strlen(p_name) + 1);
    clock_gettime(CLOCK_MONOTONIC, &after);

    diff = get_time_diff(before, after);
    set_last_work_time(&st_stats, diff);
    add_to_summary_work_time(&st_stats, diff);
    st_show_stats();

    strcpy(node->p_name, p_name); 
    node->left = NULL; 
    node->right = NULL;
    return node;
}

void st_insert_node(standard_Tree *tree, int p_id, const char *p_name)
{
    if (tree->root == NULL)
    {
        tree->root = st_create_node(p_id, p_name);
        return;
    }

    standard_Node *current = tree->root;

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
                current->left = st_create_node(p_id, p_name);
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
                current->right = st_create_node(p_id, p_name);
                return;
            }
            else
            {
                current = current->right;
            }
        }
    }
}

standard_Node* st_find_min_node(standard_Node *root) {
    if (root == NULL) {
        return NULL;
    }
    while (root->left != NULL) {
        root = root->left;
    }
    return root;
}

standard_Node* st_find_node_by_p_name(standard_Node *root, const char *p_name)
{
    if (root == NULL) {
        return NULL;
    }

    int result = strcmp(p_name, root->p_name);

    if (result == 0) {
        return root;
    } else if (result < 0) {
        return st_find_node_by_p_name(root->left, p_name);
    } else {
        return st_find_node_by_p_name(root->right, p_name);
    }
}

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

standard_Node** st_find_parent_node(standard_Node** current, standard_Node* node)
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

    standard_Node** parent = st_find_parent_node(&((*current)->left), node);
    if (parent != NULL) {
        return parent;
    }

    return st_find_parent_node(&((*current)->right), node);
}

void st_delete_node_by_name(standard_Tree *tree, standard_Node *root, const char *p_name)
{
    if (root == NULL) return;

    standard_Node *to_delete = st_find_node_by_p_name(tree->root, p_name);
    standard_Node **parent = st_find_parent_node(&tree->root, to_delete);
    st_delete_node(tree, &to_delete, parent);
}

void st_delete_node(standard_Tree *tree, standard_Node **current, standard_Node **parent)
{
    if (*current == NULL) return;

    if (parent == NULL) parent = &tree->root;

    standard_Node *to_delete = *current;

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
        standard_Node *min = st_find_min_node((*current)->right); 
        standard_Node **parent_min = st_find_parent_node(current, min); 

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

    st_log_action("[in st_btree] : freeing memory for person's name...\n");
    clock_gettime(CLOCK_MONOTONIC, &before);
    free(to_delete->p_name);
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

void st_print_tree_like_list(standard_Node *node)
{
    if (node == NULL) return;

    st_print_tree_like_list(node->left);
    printf("%02d %s\n", node->p_id, node->p_name);
    st_print_tree_like_list(node->right);
}

void st_write_tree_to_file(standard_Node *node, FILE *file)
{
    if (node == NULL) return;

    st_write_tree_to_file(node->left, file);
    fprintf(file, "%02d %s\n", node->p_id, node->p_name);
    st_write_tree_to_file(node->right, file);
}
