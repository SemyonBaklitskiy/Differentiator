#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "node_functions.h"
#include "error.h"
#include "build_tree.h"

static const char* allowedFuncs[]  = {"sin", "cos", "tg", "ctg", "ln"};
static const size_t maxFuncsLength = 3;
static const size_t minFuncsLength = 2;

static void skip_spaces(const char** s);

static char* get_function_name(const char** s);
static bool match(const char* str);
static opAndFuncType get_operator(const int symbol);
static struct Node* get_function_node(const char** s);

static struct Node* Get_E(const char** s);
static struct Node* Get_T(const char** s);
static struct Node* Get_Pow(const char** s);
static struct Node* Get_P(const char** s);
static struct Node* Get_N(const char** s);

struct Node* Get_G(const char** s) { 
    struct Node* node = Get_E(s);

    CHECK_NULL(node, NO_ERRORS, return NULL);

    if (**s == ')') {
        PRINT_ERROR(PAIR_BRACKETS);
        free_tree(node);
        return NULL;

    } else if (**s != '\0') {
        PRINT_ERROR(NOT_FOUND_OPERATOR);
        free_tree(node);
        return NULL;
    }

    return node;
}

static struct Node* Get_E(const char** s) {
    struct Node* node = NULL; 

    int amount = 0;

    struct Node* leftNode = Get_T(s);
    CHECK_NULL(leftNode, NO_ERRORS, return NULL);

    while ((**s == '+') || (**s == '-')) {
        if (amount >= 1) {
            opAndFuncType op = get_operator(**s);
            ++(*s);

            struct Node* rightNode = Get_T(s);
            CHECK_NULL(rightNode, NO_ERRORS, free_tree(node); return NULL);

            struct Node* newNode = create_operator(node->op, node->left, node->right);
            CHECK_NULL(rightNode, NO_ERRORS, free_tree(node); free_tree(rightNode); return NULL);

            node->left  = newNode;
            node->right = rightNode;
            node->op    = op;

        } else {
            opAndFuncType op = get_operator(**s);
            ++(*s);

            struct Node* rightNode = Get_T(s);
            CHECK_NULL(rightNode, NO_ERRORS, free_tree(leftNode); return NULL);

            node = create_operator(op, leftNode, rightNode);  
            CHECK_NULL(node, NO_ERRORS, free_tree(leftNode); free_tree(rightNode); return NULL);
        }

        ++amount;
    }

    if (amount == 0)
        return leftNode;

    return node;
}

static struct Node* Get_T(const char** s) {
    struct Node* node = NULL; 

    int amount = 0;

    struct Node* leftNode = Get_Pow(s);

    CHECK_NULL(leftNode, NO_ERRORS, return NULL);

    while ((**s == '*') || (**s == '/')) {
        if (amount >= 1) {
            opAndFuncType op = get_operator(**s);
            ++(*s);

            struct Node* rightNode = Get_Pow(s);
            CHECK_NULL(rightNode, NO_ERRORS, free_tree(node); return NULL);

            struct Node* newNode = create_operator(node->op, node->left, node->right);
            CHECK_NULL(rightNode, NO_ERRORS, free_tree(node); free_tree(rightNode); return NULL);

            node->left  = newNode;
            node->right = rightNode;
            node->op    = op;

        } else {
            opAndFuncType op = get_operator(**s);
            ++(*s);

            struct Node* rightNode = Get_Pow(s);
            CHECK_NULL(rightNode, NO_ERRORS, free_tree(leftNode); return NULL);

            node = create_operator(op, leftNode, rightNode);  
            CHECK_NULL(node, NO_ERRORS, free_tree(leftNode); free_tree(rightNode); return NULL);
        }

        ++amount;
    }

    if (amount == 0)
        return leftNode;

    return node;
}

static struct Node* Get_Pow(const char** s) {
    struct Node* node = NULL;

    int amount = 0;

    struct Node* leftNode = Get_P(s);

    CHECK_NULL(leftNode, NO_ERRORS, return NULL);

    while (**s == '^') {
        ++(*s);

        if (amount >= 1) {
            struct Node* rightNode = Get_P(s);
            CHECK_NULL(rightNode, NO_ERRORS, free_tree(node); return NULL);

            struct Node* newNode = create_operator(POW, node->right, rightNode);
            CHECK_NULL(newNode, NO_ERRORS, free_tree(node); free_tree(rightNode); return NULL);

            node->right = newNode;

        } else {
            struct Node* rightNode = Get_P(s);
            CHECK_NULL(rightNode, NO_ERRORS, free_tree(leftNode); return NULL);

            node = create_operator(POW, leftNode, rightNode);
            CHECK_NULL(node, NO_ERRORS, free_tree(leftNode); free_tree(rightNode); return NULL);
        }

        ++amount;
    } 

    if (amount == 0) 
        return leftNode;

    return node;
}

static struct Node* Get_P(const char** s) {
    struct Node* node = NULL;
    skip_spaces(s);

    if (**s == '(') {
        ++(*s);

        node = Get_E(s);

        CHECK_NULL(node, NO_ERRORS, return NULL);

        if (**s != ')') {
            PRINT_ERROR(PAIR_BRACKETS);
            free_tree(node);
            return NULL;
        }

        ++(*s);

    } else {
        node = Get_N(s);
    }

    skip_spaces(s);
    return node;
}

static struct Node* Get_N(const char** s) {
    struct Node* node = NULL;

    double number = 0.0;
    int amountOfSymbols = 0;

    if (sscanf(*s, "%lf %n", &number, &amountOfSymbols) > 0) {
        node = create_number(number);
        CHECK_NULL(node, NO_ERRORS, return NULL);

        *s += amountOfSymbols;

    } else if ((**s == 'x') || (**s == 'X')) {
        node = create_var(**s);
        CHECK_NULL(node, NO_ERRORS, return NULL);

        ++(*s);

    } else {
        node = get_function_node(s);
        CHECK_NULL(node, NO_ERRORS, return NULL);
    }

    return node;
}

static struct Node* get_function_node(const char** s) {
    char* functionName = get_function_name(s);
    CHECK_NULL(functionName, NO_ERRORS, return NULL);
    
    if (**s != '(') {
        PRINT_ERROR(NO_BRACKET_IN_FUNCTION);
        free(functionName);
        return NULL;
    }

    ++(*s);

    struct Node* node = NULL; 
    struct Node* leftNode = Get_E(s);

    CHECK_NULL(leftNode, NO_ERRORS, free(functionName); return NULL);

    if (strcmp(functionName, "sin") == 0) {
        node = create_function(SIN, leftNode);

    } else if (strcmp(functionName, "cos") == 0) {
        node = create_function(COS, leftNode);

    } else if (strcmp(functionName, "tg") == 0) {
        node = create_function(TG, leftNode);

    } else if (strcmp(functionName, "ctg") == 0) {
        node = create_function(CTG, leftNode);
    
    } else if (strcmp(functionName, "ln") == 0) {
        node = create_function(LN, leftNode);

    } else {
        //add functions here
    }

    CHECK_NULL(node, NO_ERRORS, free(functionName); free_tree(leftNode); return NULL);

    if (**s != ')') {
        PRINT_ERROR(PAIR_BRACKETS);
        free(functionName);
        free_tree(node);
        return NULL;
    }

    ++(*s);

    free(functionName);
    return node;
}

static bool match(const char* str) {
    for (size_t index = 0; index < (sizeof(allowedFuncs) / sizeof(allowedFuncs[0])); ++index) {
        if (strcmp(str, allowedFuncs[index]) == 0)
            return true;
    }

    return false;
}

static char* get_function_name(const char** s) {
    size_t counter = 0;
    const size_t strLength = strlen(*s);

    char* functionName = (char*)calloc(maxFuncsLength + 1, sizeof(char));
    CHECK_NULL(functionName, MEM_ERROR, return NULL);

    bool stop = false;

    while ((!stop) && (counter < maxFuncsLength) && (counter < strLength)) {
        functionName[counter] = **s;
        ++counter;
        functionName[counter] = '\0';

        if (counter >= minFuncsLength)
            stop = match(functionName);

        ++(*s);
    }

    if (!stop) {
        PRINT_ERROR(FORBIDDEN_SYMBOL);
        free(functionName);
        return NULL; 
    }

    return functionName;
}

static opAndFuncType get_operator(const int symbol) {
    switch (symbol) {
        case '+':
            return ADD;
            break;

        case '-':
            return SUB;
            break;

        case '*':
            return MUL;
            break;

        case '/':
            return DIV;
            break;

        case '^':
            return POW;
            break;

        default:
            return NOTHING;
            break;
    }
}

static void skip_spaces(const char** s) {
    while (**s == ' ')
       ++(*s);
}