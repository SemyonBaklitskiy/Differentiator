#include <stdlib.h>
#include "math_functions.h"
#include "functions.h"
#include "error.h"
#include "node_functions.h"
#include "differentiate_tree.h"

#define dup(arg) dup_node(arg)
#define diff(arg) differentiate(arg)
#define Left node->left
#define Right node->right
#define type(node) node->type
#define op(node) node->op
#define num(node) node->number

#define operator(op, left, right) create_operator(op, left, right)
#define function(func, left) create_function(func, left)
#define number(val) create_number(val)

#define isNumber(node, val) (type(node) == NUMBER) && compare(num(node), val)

#define full_delete_and_change(deleteNode, copyNode) free_tree(deleteNode); struct Node* tmp = copyNode; copy_node(node, tmp); free_tree(tmp);
#define delete_and_change(deleteNode, copyNode) free_tree(deleteNode); struct Node* tmp = copyNode; copy_node(node, tmp); free(tmp);

struct Node* differentiate(const struct Node* node) {
    switch (type(node)) {
    case NUMBER:
        return number(0.0);
        break;

    case VARIABLE:
        return number(1.0);
        break;

    case OPERATOR: 
        switch (op(node)) {
        case ADD:
            return operator(ADD, diff(Left), diff(Right));
            break;

        case SUB: 
            return operator(SUB, diff(Left), diff(Right));
            break;
        
        case MUL: 
            return operator(ADD, operator(MUL, diff(Left), dup(Right)), operator(MUL, dup(Left), diff(Right)));
            break;

        case DIV:
            return operator(DIV, operator(SUB, operator(MUL, diff(Left), dup(Right)), operator(MUL, dup(Left), diff(Right))), operator(POW, dup(Right), number(2.0)));
            break;

        case POW: 
            switch (type(Right)) {
            case NUMBER: 
                return operator(MUL, number(num(Right)), operator(MUL, operator(POW, dup(Left), number(num(Right) - 1.0)), diff(Left)));
                break;

            default: 
                struct Node* lnNode = create_function(ln, Left);
                struct Node* arg = create_operator(MUL, Right, lnNode);
                struct Node* result = create_operator(MUL, dup(node), diff(arg));

                free(lnNode);
                free(arg);

                return result;
                break;
            }

        default:
            break;
        }

        break;

    case FUNCTION: 
        switch (op(node)) {

        #define generator(function, diffRules, ...) case function: return diffRules; break;
        #include "define.h" //code generation for diff function node
        #undef generator
        
        default:
            break;
        }

        break;
    }

    PRINT_ERROR(WRONG_TYPE_OR_OP_OR_FUNC);
    exit(WRONG_TYPE_OR_OP_OR_FUNC);

    return NULL;
}

void simplify_constants(struct Node* node) {
    if (node == NULL)
        return;

    simplify_constants(Left);
    simplify_constants(Right);

    switch (type(node)) {
    case FUNCTION:
        switch (op(node)) {

        #define generator(function, diffRules, c_funcName) case function: if (type(Left) == NUMBER) { struct Node* tmp = Left; copy_node(node, tmp); num(node) = get_function(#c_funcName, num(tmp)); free_tree(tmp); } break;
        #include "define.h"
        #undef generator

        default:
            break;
        }

        break;

    case OPERATOR:
        switch (op(node)) {
        case ADD:
            if (isNumber(Left, 0.0)) {
                delete_and_change(Left, Right);

            } else if (isNumber(Right, 0.0)) {
                delete_and_change(Right, Left);
            }

            break;

        case SUB: 
            if (isNumber(Left, 0.0)) {
                num(Left) = -1.0;
                op(node) = MUL;
                simplify_constants(node);

            } else if (isNumber(Right, 0.0)) {
                delete_and_change(Right, Left);
            }

            break;

        case MUL:
            if (isNumber(Left, 0.0)) {
                full_delete_and_change(Right, Left);

            } else if (isNumber(Right, 0.0)) {
                full_delete_and_change(Left, Right);

            } else if (isNumber(Left, 1.0)) {
                delete_and_change(Left, Right);

            } else if (isNumber(Right, 1.0)) {
                delete_and_change(Right, Left);
            }

            break;

        case DIV: 
            if (isNumber(Left, 0.0)) {
                full_delete_and_change(Right, Left);

            } else if (isNumber(Right, 1.0)) {
                delete_and_change(Right, Left);
            }

            break;

        case POW: 
            if (isNumber(Left, 1.0)) {
                full_delete_and_change(Right, Left);

            } else if (isNumber(Right, 1.0)) {
                delete_and_change(Right, Left);

            } else if (isNumber(Right, 0.0)) {
                full_delete_and_change(Left, Right);
                num(node) = 1.0;

            } else if (isNumber(Right, -1.0)) {
                op(node) = DIV;
                struct Node* tmp = Left;
                Left = Right;
                num(Left) = 1.0;
                Right = tmp;
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


#undef dup
#undef diff
#undef Left
#undef Right
#undef type
#undef op
#undef num

#undef operator
#undef function
#undef number

#undef isNumber

#undef full_delete_and_change
#undef delete_and_change