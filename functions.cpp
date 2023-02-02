#include <stdlib.h>
#include <stdio.h>
#include "functions.h"
#include <assert.h>

static const char* s = NULL;

static struct Node* Get_G(const char* str);
static struct Node* Get_E();
static struct Node* Get_T();
static struct Node* Get_P();
static struct Node* Get_N();
static struct Node* create_node(const int type, const int number, const operatorType op, struct Node* left, struct Node* right);

char* get_str() {
    char* str = NULL;
    scanf("%m[^\n]s", &str);

    return str;
}

struct Node* get_tree(const char* str) {
    return Get_G(str);
}

void print_tree(struct Node* node) { //TODO do better
    if ((node->left == NULL) && (node->right == NULL)) {
        switch (node->type) {
        case VARIABLE: 
            printf("%c", node->var);
            break;

        case NUMBER: 
            printf("%d", node->number);
            break;

        default:
            assert(0);
        }

        return;
    }

    printf("(");

    print_tree(node->left);
    if (node->type == OPERATOR) {
        switch (node->op) {
        case ADD:
            printf("+");
            break;

        case SUB: 
            printf("-");
            break;

        case MUL: 
            printf("*");
            break;

        case DIV: 
            printf("/");
            break;

        default:
            assert(0);
        }
    }
    print_tree(node->right);

    printf(")");

    return;
}

struct Node* Diff(const struct Node* node) { //TODO fix
    switch (node->type) {
    case NUMBER:
        return create_node(NUMBER, 0, NOTHING, NULL, NULL);
        break;

    case VARIABLE:
        return create_node(NUMBER, 1, NOTHING, NULL, NULL);
        break;

    case OPERATOR:
        switch (node->op) {
        case ADD:
            return create_node(OPERATOR, 0, ADD, Diff(node->left), Diff(node->right));
            break;

        case SUB:
            return create_node(OPERATOR, 0, SUB, Diff(node->left), Diff(node->right)); 
            break;

        case MUL:
            return create_node(OPERATOR, 0, ADD, create_node(OPERATOR, 0, MUL, Diff(node->left), node->right), create_node(OPERATOR, 0, MUL, node->left, Diff(node->right)));
            break;

        case DIV:
            return create_node(OPERATOR, 0, DIV, create_node(OPERATOR, 0, SUB, create_node(OPERATOR, 0, MUL, Diff(node->left), node->right), create_node(OPERATOR, 0, MUL, node->left, Diff(node->right))), create_node(OPERATOR, 0, MUL, node->right, node->right));
            break;

        default:
            assert(0);
        }

    default:
        assert(0);
    }

    return NULL;
}

void free_tree(struct Node* node) {
    if (node == NULL)
        return;

    free_tree(node->left);
    free_tree(node->right);

    free(node);
    return;
}

static struct Node* Get_G(const char* str) {
    s = str;
    struct Node* node = Get_E();
    assert(*s == '\0');

    return node;
}

static struct Node* Get_E() { //TODO make better
    struct Node* node = (struct Node*)calloc(1, sizeof(Node));
    node->type = OPERATOR;

    bool hasAddOrSub = false;
    int amount = 0;

    struct Node* nodeLeft = Get_T();

    while((*s == '+') || (*s == '-')) {
        hasAddOrSub = true;

        if (amount >= 1) {
            struct Node* newNode = (struct Node*)calloc(1, sizeof(Node));
            newNode->left = node->left;
            newNode->right = node->right;
            newNode->type = OPERATOR;
            newNode->op = node->op;

            if (*s == '+') {
                node->op = ADD;
                ++amount;

            } else {
                node->op = SUB;
                ++amount;
            }

            ++s;
            struct Node* nodeRight = Get_T();

            node->left = newNode;
            node->right = nodeRight;

        } else {
            if (*s == '+') {
                node->op = ADD;
                ++amount;

            } else {
                node->op = SUB;
                ++amount;
            }

            ++s;
            struct Node* nodeRight = Get_T();

            node->left = nodeLeft;
            node->right = nodeRight;  
        }
    }

    if (!hasAddOrSub) {
        free(node);
        return nodeLeft;
    }

    return node;
}

static struct Node* Get_T() { //TODO make better
    struct Node* node = (struct Node*)calloc(1, sizeof(Node));
    node->type = OPERATOR;

    bool hasMulOrDiv = false;
    int amount = 0;

    struct Node* nodeLeft = Get_P();

    while((*s == '*') || (*s == '/')) {
        hasMulOrDiv = true;

        if (amount >= 1) {
            struct Node* newNode = (struct Node*)calloc(1, sizeof(Node));
            newNode->left = node->left;
            newNode->right = node->right;
            newNode->type = OPERATOR;
            newNode->op = node->op;

            if (*s == '*') {
                node->op = MUL;
                ++amount;

            } else {
                node->op = DIV;
                ++amount;
            }

            ++s;
            struct Node* nodeRight = Get_P();

            node->left = newNode;
            node->right = nodeRight;

        } else {
            if (*s == '*') {
                node->op = MUL;
                ++amount;

            } else {
                node->op = DIV;
                ++amount;
            }

            ++s;
            struct Node* nodeRight = Get_P();

            node->left = nodeLeft;
            node->right = nodeRight;  
        }
    }

    if (!hasMulOrDiv) {
        free(node);
        return nodeLeft;
    }

    return node;
}

static struct Node* Get_P() {
    struct Node* node = NULL;

    if (*s == '(') {
        ++s;
        node = Get_E();
        assert(*s == ')');
        ++s;

    } else {
        node = Get_N();
    }

    return node;
}

static struct Node* Get_N() {
    struct Node* node = (struct Node*)calloc(1, sizeof(Node));

    if ((*s >= '0') && (*s <= '9')) {
        int number = 0;

        while ((*s >= '0') && (*s <= '9')) {
            number = (number * 10) + (*s - '0');
            ++s;
        }

        node->left = NULL;
        node->right = NULL;
        node->type = NUMBER;
        node->number = number;

    } else if (*s == 'x') {
        node->left = NULL;
        node->right = NULL;
        node->type = VARIABLE;
        node->var = *s;

        ++s;

    } else {
        assert(0);
    }

    return node;
}

static struct Node* create_node(const int type, const int number, const operatorType op, struct Node* left, struct Node* right) {
    struct Node* node = (struct Node*)calloc(1, sizeof(Node));

    switch (type) {
    case NUMBER:
        node->type = NUMBER;
        node->number = number; 
        node->left = left;
        node->right = right;

        break;

    case VARIABLE:
        node->type = VARIABLE;
        node->var = 'x';
        node->left = left;
        node->right =right;

        break;

    case OPERATOR:
        node->type = OPERATOR;
        node->op = op;
        node->left = left;
        node->right = right;

        break;

    default:
        assert(0);
    }

    return node;
}