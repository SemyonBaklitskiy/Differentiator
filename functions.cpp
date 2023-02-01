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

char* get_str() {
    char* str = NULL;
    scanf("%m[^\n]s", &str);

    return str;
}

struct Node* get_tree(const char* str) {
    return Get_G(str);
}

static struct Node* Get_G(const char* str) {
    s = str;
    struct Node* node = Get_E();
    assert(*s == '\0');

    return node;
}

void print_tree(struct Node* node) { //TODO do better
    if ((node->left == NULL) && (node->right == NULL)) {
        if (node->type == VARIABLE) {
            printf("x");

        } else if (node->type == NUMBER) {
            printf("%d", node->number);

        } else {
            assert(0);
        }

        return;
    }

    printf("(");

    print_tree(node->left);
    if (node->type == OPERATOR) {
        if (node->op == ADD) {
            printf("+");

        } else if (node->op == SUB) {
            printf("-");

        } else if (node->op == MUL) {
            printf("*");

        } else {
            printf("/");
        }
    }
    print_tree(node->right);

    printf(")");

    return;
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

static struct Node* Get_E() {
    struct Node* node = (struct Node*)calloc(1, sizeof(Node));
    node->type = OPERATOR;

    bool hasAddOrSub = false;

    struct Node* nodeLeft = Get_T();

    while((*s == '+') || (*s == '-')) {
        hasAddOrSub = true;

        if (*s == '+') {
            node->op = ADD;

        } else {
            node->op = SUB;
        }

        ++s;

        struct Node* nodeRight = Get_T();

        node->left = nodeLeft;
        node->right = nodeRight;
    }

    if (!hasAddOrSub)
        return nodeLeft;

    return node;
}

static struct Node* Get_T() {
    struct Node* node = (struct Node*)calloc(1, sizeof(Node));
    node->type = OPERATOR;

    bool hasMulOrDiv = false;

    struct Node* nodeLeft = Get_P();

    while((*s == '*') || (*s == '/')) {
        hasMulOrDiv = true;

        if (*s == '*') {
            node->op = MUL;

        } else {
            node->op = DIV;
        }

        ++s;

        struct Node* nodeRight = Get_P();

        node->left = nodeLeft;
        node->right = nodeRight;
    }

    if (!hasMulOrDiv)
        return nodeLeft;

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