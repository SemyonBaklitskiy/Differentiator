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
static struct Node* dup_node(struct Node* node);
static struct Node* create_node(const int type, const double number, const char var, const operatorType op, struct Node* left, struct Node* right);

char* get_str() {
    char* str = NULL;
    scanf("%m[^\n]s", &str);

    return str;
}

struct Node* get_tree(const char* str) {
    return Get_G(str);
}

void print_tree(struct Node* node) { //TODO maybe can be better
    if ((node->left == NULL) && (node->right == NULL)) {
        switch (node->type) {
        case VARIABLE: 
            printf("%c", node->var);
            break;

        case NUMBER: {
            int integerNumber = (int)node->number;

            if (integerNumber == node->number) {
                printf("%d", (int)node->number);

            } else {
                printf("%lf", node->number);
            }

            break;
        }

        default:
            break;
        }

        return;
    }

    bool hasOpenBracket = false;

    if (node->type == OPERATOR) {
        switch (node->op) {
        case MUL:
            if (node->left->type == OPERATOR) {
                printf("(");
                hasOpenBracket = true;
            }
            break;

        case DIV:
            if (node->left->type == OPERATOR) {
                printf("(");
                hasOpenBracket = true;
            }
            break;

        default:
            break;
        }
    }

    print_tree(node->left);

    if (hasOpenBracket) {
        printf(")");
        hasOpenBracket = false;
    }

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
            break;
        }
    }

    if (node->right->type == OPERATOR) {
        printf("(");
        hasOpenBracket = true;
    }

    print_tree(node->right);

    if (hasOpenBracket) {
        printf(")");
        hasOpenBracket = false;
    }

    return;
}

struct Node* Diff(const struct Node* node) { //TODO DSL
    switch (node->type) {
    case NUMBER:
        return create_node(NUMBER, 0, node->var, NOTHING, NULL, NULL);
        break;

    case VARIABLE:
        return create_node(NUMBER, 1, node->var, NOTHING, NULL, NULL);
        break;

    case OPERATOR: 
        switch (node->op) {
        case ADD: {
            struct Node* dupLeftNode = dup_node(node->left);
            struct Node* dupRightNode = dup_node(node->right);

            struct Node* result = create_node(OPERATOR, 0, node->var, ADD, Diff(dupLeftNode), Diff(dupRightNode));

            free_tree(dupLeftNode);
            free_tree(dupRightNode);

            return result;
            break;
        }

        case SUB: {
            struct Node* dupLeftNode = dup_node(node->left);
            struct Node* dupRightNode = dup_node(node->right);

            struct Node* result = create_node(OPERATOR, 0, node->var, SUB, Diff(dupLeftNode), Diff(dupRightNode));

            free_tree(dupLeftNode);
            free_tree(dupRightNode);

            return result;
            break;
        }

        case MUL: {
            struct Node* dupLeftNode = dup_node(node->left);
            struct Node* dupRightNode = dup_node(node->right);

            struct Node* result = create_node(OPERATOR, 0, node->var, ADD, create_node(OPERATOR, 0, node->var, MUL, Diff(dupLeftNode), dup_node(node->right)), create_node(OPERATOR, 0, node->var, MUL, dup_node(node->left), Diff(dupRightNode)));

            free_tree(dupLeftNode);
            free_tree(dupRightNode);

            return result;
            break;
        }

        case DIV: {
            struct Node* dupLeftNode = dup_node(node->left);
            struct Node* dupRightNode = dup_node(node->right);

            struct Node* result = create_node(OPERATOR, 0, node->var, DIV, create_node(OPERATOR, 0, node->var, SUB, create_node(OPERATOR, 0, node->var, MUL, Diff(dupLeftNode), dup_node(node->right)), create_node(OPERATOR, 0, node->var, MUL, dup_node(node->left), Diff(dupRightNode))), create_node(OPERATOR, 0, node->var, MUL, dup_node(node->right), dup_node(node->right)));

            free_tree(dupLeftNode);
            free_tree(dupRightNode);

            return result;
            break;
        }

        case NOTHING:
            break; 
        }
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
    double number = 0.0;
    int amountOfSymbols = 0;

    if (sscanf(s, "%lf %n", &number, &amountOfSymbols) > 0) {
        node->left = NULL;
        node->right = NULL;
        node->type = NUMBER;
        node->number = number;

        s+= amountOfSymbols;

    } else if (((*s >= 'a') && (*s <= 'z')) || ((*s >= 'A') && (*s <= 'Z'))) {
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

static struct Node* create_node(const int type, const double number, const char var, const operatorType op, struct Node* left, struct Node* right) {
    struct Node* node = (struct Node*)calloc(1, sizeof(Node));

    switch (type) {
    case NUMBER:
        node->type = NUMBER;
        node->number = number; 
        node->op = op;
        node->var = var;
        node->left = left;
        node->right = right;

        break;

    case VARIABLE:
        node->type = VARIABLE;
        node->var = var;
        node->number = number;
        node->op = op;
        node->left = left;
        node->right =right;

        break;

    case OPERATOR:
        node->type = OPERATOR;
        node->op = op;
        node->left = left;
        node->number = number;
        node->var = var;
        node->right = right;

        break;

    default:
        assert(0);
    }

    return node;
}

static struct Node* dup_node(struct Node* node) {
    if (node == NULL)
        return NULL;

    struct Node* newNode = (struct Node*)calloc(1, sizeof(Node));

    newNode->number = node->number;
    newNode->op = node->op;
    newNode->type = node->type;
    newNode->var = node->var;

    newNode->left = dup_node(node->left);
    newNode->right = dup_node(node->right);

    return newNode;
}