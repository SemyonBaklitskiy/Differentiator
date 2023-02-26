#include <stdio.h>
#include "functions.h"
#include "error.h"
#include "output_tree.h"

void print_tree(const struct Node* node) {
    if (node == NULL) {
        PRINT_ERROR(OUTPUT_ERROR);
        return;
    }

    switch (node->type) {
    case NUMBER:
        if (node->number < 0)
            printf("(");

        if ((int)node->number == node->number) {
            printf("%d", (int)node->number);

        } else {
            printf("%lf", node->number);
        }

        if (node->number < 0)
            printf(")");

        break;

    case VARIABLE:
        printf("%c", node->var);
        break;

    case FUNCTION:
        switch (node->op) {
        case SIN:
            printf("sin(");
            print_tree(node->left);
            printf(")");

            break;

        case COS:
            printf("cos(");
            print_tree(node->left);
            printf(")");

            break;

        case TG:
            printf("tg(");
            print_tree(node->left);
            printf(")");

            break;

        case CTG:
            printf("ctg(");
            print_tree(node->left);
            printf(")");

            break;

        case LN:
            printf("ln(");
            print_tree(node->left);
            printf(")");

            break;

        default:
            PRINT_ERROR(OUTPUT_ERROR);
            return;
            break;    
        }

        break;

    case OPERATOR:
        switch (node->op) {
        case ADD:
            print_tree(node->left);
            printf("+");
            print_tree(node->right);

            break;

        case SUB:
            print_tree(node->left);
            printf("-");
            print_tree(node->right);

            break;

        case MUL:
            CHECK_NULL(node->left, OUTPUT_ERROR, return);
            CHECK_NULL(node->right, OUTPUT_ERROR, return);

            if (((node->left->op == ADD) || (node->left->op == SUB)) && ((node->right->op == ADD) || (node->right->op == SUB))) {
                printf("(");
                print_tree(node->left);
                printf(")*(");
                print_tree(node->right);
                printf(")");

            } else if ((node->left->op == ADD) || (node->left->op == SUB)) {
                printf("(");
                print_tree(node->left);
                printf(")*");
                print_tree(node->right);

            } else if ((node->right->op == ADD) || (node->right->op == SUB)) {
                print_tree(node->left);
                printf("*(");
                print_tree(node->right);
                printf(")");

            } else {
                print_tree(node->left);
                printf("*");
                print_tree(node->right);
            }

            break;

        case DIV: 
            CHECK_NULL(node->left, OUTPUT_ERROR, return);
            CHECK_NULL(node->right, OUTPUT_ERROR, return);

            if (((node->left->op == ADD) || (node->left->op == SUB)) && ((node->right->op == ADD) || (node->right->op == SUB))) {
                printf("(");
                print_tree(node->left);
                printf(")/(");
                print_tree(node->right);
                printf(")");

            } else if ((node->left->op == ADD) || (node->left->op == SUB)) {
                printf("(");
                print_tree(node->left);
                printf(")/");
                print_tree(node->right);

            } else if ((node->right->op == ADD) || (node->right->op == SUB)) {
                print_tree(node->left);
                printf("/(");
                print_tree(node->right);
                printf(")");

            } else {
                print_tree(node->left);
                printf("/");
                print_tree(node->right);
            }

            break;

        case POW:
            CHECK_NULL(node->left, OUTPUT_ERROR, return);
            CHECK_NULL(node->right, OUTPUT_ERROR, return);

            if (((node->left->op == ADD) || (node->left->op == SUB) || (node->left->op == MUL) || (node->left->op == DIV)) && 
                ((node->right->op == ADD) || (node->right->op == SUB) || (node->right->op == MUL) || (node->right->op == DIV))) {
                    printf("(");
                    print_tree(node->left);
                    printf(")^(");
                    print_tree(node->right);
                    printf(")");
                }

            else if ((node->left->op == ADD) || (node->left->op == SUB) || (node->left->op == MUL) || (node->left->op == DIV)) {
                printf("(");
                print_tree(node->left);
                printf(")^");
                print_tree(node->right);

            } else if ((node->right->op == ADD) || (node->right->op == SUB) || (node->right->op == MUL) || (node->right->op == DIV)) {
                print_tree(node->left);
                printf("^(");
                print_tree(node->right);
                printf(")");

            } else {
                print_tree(node->left);
                printf("^");
                print_tree(node->right);
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