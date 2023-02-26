#include <stdlib.h>
#include <math.h>
#include "functions.h"
#include "error.h"
#include "node_functions.h"
#include "differentiate_tree.h"

#define diff_mul(left, right) create_operator(ADD, create_operator(MUL, leftNode, dup_node(right)), create_operator(MUL, dup_node(left), rightNode))
#define diff_div(left, right) create_operator(DIV, create_operator(SUB, create_operator(MUL, leftNode, dup_node(right)), create_operator(MUL, dup_node(left), rightNode)), create_operator(POW, dup_node(right), create_number(2)))
#define diff_pow_num(left, right) create_operator(MUL, create_number(right->number), create_operator(MUL, create_operator(POW, dup_node(left), create_number(right->number - 1.0)), leftNode))
#define diff_tg(left) create_operator(MUL, create_operator(DIV, create_number(1.0), create_operator(POW, create_function(COS, dup_node(left)), create_number(2.0))), leftNode)
#define diff_ctg(left) create_operator(MUL, create_operator(DIV, create_number(-1.0), create_operator(POW, create_function(SIN, dup_node(left)), create_number(2.0))), leftNode)

static bool compare(const double firstNumber, const double secondNumber);

struct Node* differentiate(const struct Node* node) {
    switch (node->type) {
    case NUMBER:
        return create_number(0);
        break;

    case VARIABLE:
        return create_number(1);
        break;

    case OPERATOR: 
        switch (node->op) {
        case ADD: {
            struct Node* leftNode = differentiate(node->left);
            CHECK_NULL(leftNode, NO_ERRORS, return NULL);

            struct Node* rightNode = differentiate(node->right);
            CHECK_NULL(rightNode, NO_ERRORS, free_tree(leftNode); return NULL);

            return create_operator(ADD, leftNode, rightNode);
            break;
        }

        case SUB: {
            struct Node* leftNode = differentiate(node->left);
            CHECK_NULL(leftNode, NO_ERRORS, return NULL);

            struct Node* rightNode = differentiate(node->right);
            CHECK_NULL(rightNode, NO_ERRORS, free_tree(leftNode); return NULL);

            return create_operator(SUB, leftNode, rightNode);
            break;
        }
        
        case MUL: {
            struct Node* leftNode = differentiate(node->left);
            CHECK_NULL(leftNode, NO_ERRORS, return NULL);

            struct Node* rightNode = differentiate(node->right);
            CHECK_NULL(rightNode, NO_ERRORS, free_tree(leftNode); return NULL);

            return diff_mul(node->left, node->right);
            break;
        }

        case DIV: { 
            struct Node* leftNode = differentiate(node->left);
            CHECK_NULL(leftNode, NO_ERRORS, return NULL);

            struct Node* rightNode = differentiate(node->right);
            CHECK_NULL(rightNode, NO_ERRORS, free_tree(leftNode); return NULL);

            return diff_div(node->left, node->right);
            break;
        }

        case POW: 
            switch (node->right->type) {
            case NUMBER: { 
                struct Node* leftNode = differentiate(node->left);
                CHECK_NULL(leftNode, NO_ERRORS, return NULL);
                
                return diff_pow_num(node->left, node->right);
                break;
            }

            default: 
                struct Node* ln = create_function(LN, node->left);
                struct Node* arg = create_operator(MUL, node->right, ln);

                struct Node* diffArg =  differentiate(arg);
                CHECK_NULL(diffArg, NO_ERRORS, free(ln); free(arg); return NULL);

                struct Node* result = create_operator(MUL, dup_node(node), diffArg);

                free(ln);
                free(arg);
                
                return result;
                break;
            }

        default:
            break;
        }

        break;

    case FUNCTION: {
        struct Node* leftNode = differentiate(node->left);
        CHECK_NULL(leftNode, NO_ERRORS, return NULL);

        switch (node->op) {
        case SIN: 
            return create_operator(MUL, create_function(COS, dup_node(node->left)), leftNode);
            break;

        case COS: 
            return create_operator(MUL, create_operator(MUL, create_number(-1.0), create_function(SIN, dup_node(node->left))), leftNode);
            break;

        case TG:
            return diff_tg(node->left);
            break;

        case CTG: 
            return diff_ctg(node->left);
            break;

        case LN: 
            return create_operator(MUL, create_operator(DIV, create_number(1.0), dup_node(node->left)), leftNode); 
            break;

        default:
            free_tree(leftNode);
            break;
        }

    }   break;
    }

    PRINT_ERROR(WRONG_TYPE_OR_OP_OR_FUNC);
    return NULL;
}

void simplify_constants(struct Node* node) {
    if (node == NULL)
        return;

    simplify_constants(node->left);
    simplify_constants(node->right);

    switch (node->type) {
    case OPERATOR:
        switch (node->op) {
        case ADD:
            if ((node->left->type == NUMBER) && compare(node->left->number, 0.0)) {
                free_tree(node->left);
                struct Node* rightNode = node->right;
                copy_node(node, rightNode);
                free(rightNode);

            } else if ((node->right->type == NUMBER) && compare(node->right->number, 0.0)) {
                free_tree(node->right);
                struct Node* leftNode = node->left;
                copy_node(node, leftNode);
                free(leftNode);
            }

            break;

        case SUB: 
            if ((node->left->type == NUMBER) && compare(node->left->number, 0.0)) {
                node->left->number = -1.0;
                node->op = MUL;

            } else if ((node->right->type == NUMBER) && compare(node->right->number, 0.0)) {
                free_tree(node->right);
                struct Node* leftNode = node->left;
                copy_node(node, leftNode);
                free(leftNode);
            }

            break;

        case MUL:
            if ((node->left->type == NUMBER) && compare(node->left->number, 0.0)) {
                free_tree(node->right);
                struct Node* leftNode = node->left;
                copy_node(node, leftNode);
                free_tree(leftNode);

            } else if ((node->right->type == NUMBER) && compare(node->right->number, 0.0)) {
                free_tree(node->left);
                struct Node* rightNode = node->right;
                copy_node(node, rightNode);
                free_tree(rightNode);

            } else if ((node->left->type == NUMBER) && compare(node->left->number, 1.0)) {
                free_tree(node->left);
                struct Node* rightNode = node->right;

                copy_node(node, rightNode);
                free(rightNode);

            } else if ((node->right->type == NUMBER) && compare(node->right->number, 1.0)) {
                free_tree(node->right);
                struct Node* leftNode = node->left;

                copy_node(node, leftNode);
                free(leftNode);
            }

            break;

        case DIV: 
            if ((node->left->type == NUMBER) && compare(node->left->number, 0.0)) {
                free_tree(node->right);
                struct Node* leftNode = node->left;
                copy_node(node, leftNode);
                free_tree(leftNode);

            } else if ((node->right->type == NUMBER) && compare(node->right->number, 1.0)) {
                free_tree(node->right);
                struct Node* leftNode = node->left;

                copy_node(node, leftNode);
                free(leftNode);
            }

            break;

        case POW: 
            if ((node->left->type == NUMBER) && compare(node->left->number, 1.0)) {
                free_tree(node->right);
                struct Node* leftNode = node->left;
                copy_node(node, leftNode);
                free_tree(leftNode);

            } else if ((node->right->type == NUMBER) && compare(node->right->number, 1.0)) {
                free_tree(node->right);
                struct Node* leftNode = node->left;
                copy_node(node, leftNode);
                free(leftNode);

            } else if ((node->right->type == NUMBER) && compare(node->right->number, -1.0)) {
                node->op = DIV;
                struct Node* leftNode = node->left;
                node->left = node->right;
                node->left->number = 1.0;
                node->right = leftNode;

            }
            break;

        default:
            break;
        }

    default:
        return;
        break;
    }
}

static bool compare(const double firstNumber, const double secondNumber) {
    const double epsilon = 0.00001;
    return (fabs(firstNumber - secondNumber) <= epsilon);
}