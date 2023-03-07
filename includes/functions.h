#pragma once

enum nodeType {
    VARIABLE     = 1,
    NUMBER       = 2,
    OPERATOR     = 3,
    FUNCTION     = 4,
};

enum opAndFuncType {
    NOTHING = 0,
    ADD     = 1,
    SUB     = 2,
    MUL     = 3,
    DIV     = 4,
    POW     = 5,
    #define generator(funcName, func, ...) func,
    #include "define.h" //code generation for enum
    #undef generator 
};

struct Node {
    nodeType type;

    double number;
    opAndFuncType op;
    char var;
    
    struct Node* left;
    struct Node* right;
};

char* get_str();
struct Node* get_tree(const char* str);
void output(const struct Node* node);
void dump(const struct Node* tree, const char* fileName);
struct Node* get_diff_tree(const struct Node* tree);
void free_tree(struct Node* node);
void free_memory(struct Node* tree, struct Node* diffTree, char* str);

#define RETURN_IF_NULL(pointer, ...) if (pointer == NULL) { __VA_ARGS__; }