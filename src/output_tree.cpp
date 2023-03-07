#include <stdio.h>
#include "functions.h"
#include "error.h"
#include "output_tree.h"

#define isInt (int)num(node) == num(node)

#define type(node) node->type
#define op(node) node->op
#define num(node) node->number
#define var(node) node->var

#define Left node->left
#define Right node->right

#define print_function(func) printf(func"("); print_tree(Left); printf(")");
#define print_operator(openBracket, symbol, closeBracket); printf(openBracket); print_tree(Left); printf(symbol); print_tree(Right); printf(closeBracket)

static const char* get_string(opAndFuncType val);
static void make_edges(const struct Node* node, FILE* stream, bool* secondCall);
static void make_labels(const struct Node* node, FILE* stream); 

void print_tree(const struct Node* node) {
    if (node == NULL) {
        PRINT_ERROR(OUTPUT_ERROR);
        return;
    }

    switch (type(node)) {
    case NUMBER:
        if (num(node) < 0)
            printf("(");

        if (isInt) {
            printf("%d", (int)num(node));

        } else {
            printf("%lf", num(node));
        }

        if (num(node) < 0)
            printf(")");

        break;

    case VARIABLE:
        printf("%c", var(node));
        break;

    case FUNCTION:
        switch (op(node)) {

        #define generator(nameFunc, func, ...) case func: print_function(nameFunc); break;
        #include "define.h" //code generation for print function node
        #undef generator

        default:
            PRINT_ERROR(OUTPUT_ERROR);
            return;
            break;    
        }

        break;

    case OPERATOR:
        switch (op(node)) {
        case ADD:
            print_operator("", "+", "");
            break;

        case SUB:
            if (op(Right) == SUB) {
                print_operator("", "-(", ")");
            } else {
                print_operator("", "-", "");
            }
            break;

        case MUL:
            CHECK_NULL(Left, OUTPUT_ERROR, return);
            CHECK_NULL(Right, OUTPUT_ERROR, return);

            if (((op(Left) == ADD) || (op(Left) == SUB)) && ((op(Right) == ADD) || (op(Right) == SUB))) {
                print_operator("(", ")*(", ")");

            } else if ((op(Left) == ADD) || (op(Left) == SUB)) {
                print_operator("(" ,")*", "");

            } else if ((op(Right) == ADD) || (op(Right) == SUB)) {
                print_operator("" ,"*(", ")");

            } else {
                print_operator("", "*", "");
            }

            break;

        case DIV: 
            CHECK_NULL(Left, OUTPUT_ERROR, return);
            CHECK_NULL(Right, OUTPUT_ERROR, return);

            if (((op(Left) == ADD) || (op(Left) == SUB)) && ((op(Right) == ADD) || (op(Right) == SUB))) {
                print_operator("(", ")/(", ")");

            } else if ((op(Left) == ADD) || (op(Left) == SUB)) {
                print_operator("(" ,")/", "");

            } else if ((op(Right) == ADD) || (op(Right) == SUB) || (op(Right) == MUL) || (op(Right) == DIV)) {
                print_operator("" ,"/(", ")");

            } else {
                print_operator("", "/", "");
            }

            break;

        case POW:
            CHECK_NULL(Left, OUTPUT_ERROR, return);
            CHECK_NULL(Left, OUTPUT_ERROR, return);

            if (((op(Left) == ADD) || (op(Left) == SUB) || (op(Left) == MUL) || (op(Left) == DIV)) && 
                ((op(Right) == ADD) || (op(Right) == SUB) || (op(Right) == MUL) || (op(Right) == DIV))) {
                    print_operator("(", ")^(", ")");;
                }

            else if ((op(Left) == ADD) || (op(Left) == SUB) || (op(Left) == MUL) || (op(Left) == DIV)) {
                print_operator("(" ,")^", "");

            } else if ((op(Right) == ADD) || (op(Right) == SUB) || (op(Right) == MUL) || (op(Right) == DIV)) {
                print_operator("" ,"^(", ")");

            } else {
                print_operator("", "^", "");
            }

            break;

        default:
            PRINT_ERROR(OUTPUT_ERROR);
            return;
            break;
        }

        break;

    default:
        PRINT_ERROR(OUTPUT_ERROR);
        return;
        break;
    }
}

void dump_tree(const struct Node* tree, const char* fileName) {
    FILE* file = fopen(fileName, "w");
    CHECK_NULL(file, FILE_ERROR, return);

    fprintf(file, "digraph Tree {\n");
    make_labels(tree, file);
    bool secondCall = false;
    make_edges(tree, file, &secondCall);

    fprintf(file, "}");
    fclose(file);
}

static void make_labels(const struct Node* node, FILE* stream) {
    if (node == NULL) 
        return;

    fprintf(stream, "\"%p\" [label = ", node);

    switch (type(node)) {
    case NUMBER:
        if (isInt) {
            fprintf(stream, "%d];\n", (int)num(node));

        } else {
            fprintf(stream, "%lf];\n", num(node));
        }
        break;

    case VARIABLE:
        fprintf(stream, "%c];\n", var(node));
        break;

    default:
        fprintf(stream, "%s];\n", get_string(op(node)));
        break;
    }

    make_labels(Left, stream);
    make_labels(Right, stream);
} 

static void make_edges(const struct Node* node, FILE* stream, bool* secondCall) {
    if (node == NULL)
        return;

    if (type(node) == FUNCTION) {
        fprintf(stream, "\"%p\" -> ", node);
        make_edges(Left, stream, secondCall);
        *secondCall = true;

    } else if ((Left == NULL) || (Right == NULL)) {
        fprintf(stream, "\"%p\";\n", node);

    } else {
        fprintf(stream, "\"%p\" -> ", node);
    }

    if (*secondCall)
        return;

    make_edges(Left, stream, secondCall);

    *secondCall = true;
    make_edges(node, stream, secondCall);
    *secondCall = false;

    make_edges(Right, stream, secondCall);
}

static const char* get_string(opAndFuncType val) {
    switch (val) {
    case ADD:
        return "\"+\"";
        break;

    case SUB:
        return "\"-\"";
        break;

    case MUL:
        return "\"*\"";
        break;

    case DIV:
        return "\"/\"";
        break;

    case POW:
        return "\"^\"";
        break;

    #define generator(nameFunc, func, ...) case func: return nameFunc; break;
    #include "define.h" //code generation for dump function node
    #undef generator

    default:
        PRINT_ERROR(WRONG_TYPE_OR_OP_OR_FUNC);
        return "\0";
        break;
    }
}

#undef isInt

#undef type
#undef var
#undef op
#undef num

#undef Left
#undef Right

#undef print_function
#undef print_operator