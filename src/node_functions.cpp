#include <stdlib.h>
#include "functions.h"
#include "node_functions.h"
#include "error.h"

struct Node* create_node(const nodeType type, const double number, const char var, const opAndFuncType op, struct Node* left, struct Node* right) {
    struct Node* node = NULL;
    node = (struct Node*)calloc(1, sizeof(Node));

    node->type   = type;
    node->number = number;
    node->var    = var;
    node->op     = op;
    node->left   = left;
    node->right  = right;

    return node;
}

struct Node* dup_node(const struct Node* node) {
    if (node == NULL)
        return NULL;
    
    struct Node* newNode = (struct Node*)calloc(1, sizeof(Node));

    newNode->number = node->number;
    newNode->op     = node->op;
    newNode->type   = node->type;
    newNode->var    = node->var;
    newNode->left   = dup_node(node->left);
    newNode->right  = dup_node(node->right);

    return newNode;
}

void copy_node(struct Node* dest, const struct Node* src) {
    dest->type   = src->type;
    dest->number = src->number;
    dest->var    = src->var;
    dest->op     = src->op;
    dest->left   = src->left;
    dest->right  = src->right;
}