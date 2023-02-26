#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include "functions.h"
#include "error.h"

//TODO make check for args function
//TODO make print function
//TODO check for null in diff function

static const char* allowedFuncs[]  = {"sin", "cos", "tg", "ctg", "ln"};
static const size_t maxFuncsLength = 3;
static const size_t minFuncsLength = 2;

static void clean_stdinput();
static char* get_function_name(const char** s);
static bool match(const char* str);
static opAndFuncType get_operator(const int symbol);

static struct Node* get_function_node(const char** s);
static struct Node* Get_G(const char** str);
static struct Node* Get_E(const char** s);
static struct Node* Get_T(const char** s);
static struct Node* Get_Pow(const char** s);
static struct Node* Get_P(const char** s);
static struct Node* Get_N(const char** s);

static struct Node* dup_node(const struct Node* node);
static struct Node* create_node(const nodeType type, const double number, const char var, const opAndFuncType op, struct Node* left, struct Node* right);
static struct Node* differentiate(const struct Node* node);

static void copy_node(struct Node* dest, const struct Node* src);
static bool compare(const double firstNumber, const double secondNumber);
static void simplify_constants(struct Node* node); 

#define PRINT_ERROR(error) processor_of_errors(error, __FILE__, __PRETTY_FUNCTION__, __LINE__)
#define CHECK_NULL(pointer, error, ...) if (pointer == NULL) { PRINT_ERROR(error); __VA_ARGS__; }

#define create_number(number) create_node(NUMBER, number, 0, NOTHING, NULL, NULL)
#define create_var(var) create_node(VARIABLE, 0.0, var, NOTHING, NULL, NULL)
#define create_operator(op, left, right) create_node(OPERATOR, 0.0, 0, op, left, right)
#define create_function(func, left) create_node(FUNCTION, 0.0, 0, func, left, NULL)
#define diff_mul(left, right) create_operator(ADD, create_operator(MUL, leftNode, dup_node(right)), create_operator(MUL, dup_node(left), rightNode))
#define diff_div(left, right) create_operator(DIV, create_operator(SUB, create_operator(MUL, leftNode, dup_node(right)), create_operator(MUL, dup_node(left), rightNode)), create_operator(POW, dup_node(right), create_number(2)))
#define diff_pow_num(left, right) create_operator(MUL, create_number(right->number), create_operator(MUL, create_operator(POW, dup_node(left), create_number(right->number - 1.0)), leftNode))
#define diff_tg(left) create_operator(MUL, create_operator(DIV, create_number(1.0), create_operator(POW, create_function(COS, dup_node(left)), create_number(2.0))), leftNode)
#define diff_ctg(left) create_operator(MUL, create_operator(DIV, create_number(-1.0), create_operator(POW, create_function(SIN, dup_node(left)), create_number(2.0))), leftNode)

char* get_str() {
    char* str = NULL;
    printf("Enter f(x) or Q (q) to quit the programm.\n");
    printf("f(x) = ");
    scanf("%m[^\n]s", &str);

    while (str == NULL) {
        printf("Please enter f(x) or enter Q (q) to quit the programm\n");
        printf("f(x) = ");

        clean_stdinput();
        scanf("%m[^\n]s", &str);

        if (str == NULL) {
            continue;

        } else if ((strcmp(str, "q") == 0) || (strcmp(str, "Q") == 0)) {
            free(str);
            return NULL;
        }
    }

    if ((strcmp(str, "q") == 0) || (strcmp(str, "Q") == 0)) {
        free(str);
        return NULL;
    }

    return str;
}

struct Node* get_tree(const char* str) {
    CHECK_NULL(str, NULL_GIVEN, return NULL);
    return Get_G(&str);
}

void print_tree(struct Node* node) { //TODO make better
    if (node == NULL) {
        printf(")");
        return;
    }

    if (node->type == FUNCTION) {
        switch (node->op) {
        case SIN:
            printf("sin(");
            break;

        case COS:
            printf("cos(");
            break;

        case TG:
            printf("tg(");
            break;

        case CTG: 
            printf("ctg(");
            break;

        case LN: 
            printf("ln(");
            break;

        default:
            break;
        }
    }

    if ((node->type == VARIABLE) || (node->type == NUMBER)) {
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

        case POW:
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

        case POW:
            printf("^");
            break;

        default:
            break;
        }
    }

    if ((node->type != FUNCTION) && (node->right->type == OPERATOR)) {
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

struct Node* get_diff_tree(const struct Node* node) { //TODO check tree for correct args
    CHECK_NULL(node, NULL_GIVEN, return NULL);
    struct Node* result = differentiate(node);
    simplify_constants(result);

    return result;
}

static struct Node* differentiate(const struct Node* node) { //TODO check for dup_node ret NULL
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

static void simplify_expression(struct Node* node) {
    if (node == NULL)
        return;

    simplify_expression(node->left);
    simplify_expression(node->right);
}

static void simplify_constants(struct Node* node) {
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

void free_tree(struct Node* node) {
    if (node == NULL)
        return;

    free_tree(node->left);
    free_tree(node->right);

    free(node);
    //node = NULL; 
    return;
}

static struct Node* Get_G(const char** s) { //TODO make (x+1)(x+2) mb here
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

static struct Node* create_node(const nodeType type, const double number, const char var, const opAndFuncType op, struct Node* left, struct Node* right) {
    struct Node* node = (struct Node*)calloc(1, sizeof(Node));
    CHECK_NULL(node, MEM_ERROR, return NULL);

    node->type   = type;
    node->number = number;
    node->var    = var;
    node->op     = op;
    node->left   = left;
    node->right  = right;

    return node;
}

static struct Node* dup_node(const struct Node* node) { //TODO make normal exit
    if (node == NULL)
        return NULL;

    struct Node* newNode = (struct Node*)calloc(1, sizeof(Node));
    CHECK_NULL(newNode, MEM_ERROR, exit(MEM_ERROR));

    newNode->number = node->number;
    newNode->op     = node->op;
    newNode->type   = node->type;
    newNode->var    = node->var;

    newNode->left   = dup_node(node->left);
    newNode->right  = dup_node(node->right);

    return newNode;
}

static void copy_node(struct Node* dest, const struct Node* src) {
    dest->type   = src->type;
    dest->number = src->number;
    dest->var    = src->var;
    dest->op     = src->op;
    dest->left   = src->left;
    dest->right  = src->right;
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

static void clean_stdinput() {
    int str = getchar();
    while ((str != EOF) && (str != '\n') && (str != '\0'))
        str = getchar();
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

static bool compare(const double firstNumber, const double secondNumber) {
    const double epsilon = 0.00001;
    return (fabs(firstNumber - secondNumber) <= epsilon);
}