#ifndef STANDARD_BTREE_H
#define STANDARD_BTREE_H

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "statistic.h"

extern statistic st_stats;

typedef struct standard_Node
{
    int p_id;                   
    char* p_name;               
    struct standard_Node *left; 
    struct standard_Node *right;
} standard_Node;

typedef struct standard_Tree
{
    struct standard_Node* root;    
} standard_Tree;

void st_log_action(const char *message);

void st_show_stats();

standard_Tree* st_tree_init();

standard_Node* st_create_node(int p_id, const char *p_name);

void st_insert_node(standard_Tree *tree, int p_id, const char *p_name);

standard_Node* st_find_min_node(standard_Node *root);

standard_Node* st_find_node_by_p_name(standard_Node *root, const char *p_name);

standard_Node* st_find_node_by_id(standard_Node *root, int p_id);

standard_Node** st_find_parent_node(standard_Node **current, standard_Node *node);

void st_delete_node(standard_Tree *tree, standard_Node **current, standard_Node **parent);

void st_delete_node_by_name(standard_Tree *tree, standard_Node *root, const char *p_name);

void st_delete_tree(standard_Tree *tree, standard_Node **node);

void st_print_tree_like_list(standard_Node *node);

void st_write_tree_to_file(standard_Node *node, FILE *file);

#endif /* STANDARD_BTREE_H */
