#ifndef MY_BTREE_H
#define MY_BTREE_H

#define _POSIX_C_SOURCE 200809L
#define _XOPEN_SOURCE 700

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "statistic.h"
#include "my_alloc.h"

typedef struct Node
{
    int p_id;           
    char *p_name;       
    struct Node *left;  
    struct Node *right; 
} Node;

typedef struct Tree
{
    struct Node *root;     
    struct mem_pool *pool; 
} Tree;

void log_action(const char *message);

void show_stats();

Tree* tree_init(struct mem_pool *pool);

Node* create_node(struct mem_pool *pool, int p_id, const char *p_name);

void insert_node(Tree *tree, int p_id, const char *p_name);

Node* find_min_node(Node *root);

Node* find_node_by_p_name(Node *root, const char *p_name);

Node* find_node_by_id(Node *root, int p_id);

Node** find_parent_node(Node **current, Node *node);

void delete_node(Tree *tree, Node **current, Node **parent);

void delete_node_by_name(Tree *tree, Node *root, const char *p_name);

void delete_tree(Tree *tree, Node **node);

void print_tree_like_list(Node *node);

void write_tree_to_file(Node *node, FILE *file);

#endif /* MY_BTREE_H */
