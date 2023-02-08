#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>
#include "functions.h"
#include "error.h"

static const char* s = NULL;
static const char* allowedFunctionNames[] = {"sin", "cos", "tg", "ctg", "ln"};
static const size_t maxLengthOfFunctionNames = 3;

static void clean_stdinput();
static char* get_function_name();
static bool match(const char* str);
static operatorType get_operator(const int symbol);

static struct Node* get_function_node();
static struct Node* Get_G(const char* str);
static struct Node* Get_E();
static struct Node* Get_T();
static struct Node* Get_Pow();
static struct Node* Get_P();
static struct Node* Get_N();

static struct Node* dup_node(struct Node* node);
static struct Node* create_node(const nodeType type, const double number, const char var, const operatorType op, struct Node* left, struct Node* right);

#define PRINT_ERROR(error) processor_of_errors(error, __FILE__, __PRETTY_FUNCTION__, __LINE__)
#define CHECK_NULL(pointer, error, ...) if (pointer == NULL) { PRINT_ERROR(error); __VA_ARGS__; }

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
    return Get_G(str);
}

void print_tree(struct Node* node) { //TODO maybe can be better
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

struct Node* Diff(const struct Node* node) { //TODO DSL and check x^x and add more functions
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

            struct Node* result = create_node(OPERATOR, 0, node->var, DIV, create_node(OPERATOR, 0, node->var, SUB, create_node(OPERATOR, 0, node->var, MUL, Diff(dupLeftNode), dup_node(node->right)), create_node(OPERATOR, 0, node->var, MUL, dup_node(node->left), Diff(dupRightNode))), create_node(OPERATOR, 0, node->var, POW, dup_node(node->right), create_node(NUMBER, 2.0, node->var, NOTHING, NULL, NULL)));

            free_tree(dupLeftNode);
            free_tree(dupRightNode);

            return result;
            break;
        }

        case POW: {
            switch (node->right->type) {
                case NUMBER: {
                    struct Node* result = (struct Node*)calloc(1, sizeof(Node));
                    result->type = OPERATOR;
                    result->op = MUL;

                    struct Node* arg = dup_node(node->left);

                    struct Node* difRightNode = (struct Node*)calloc(1, sizeof(Node));
                    difRightNode->type = OPERATOR;
                    difRightNode->op = POW;
                    difRightNode->left = arg;
                    difRightNode->right = create_node(NUMBER, node->right->number - 1.0, node->var, NOTHING, NULL, NULL);

                    struct Node* difLeftNode = create_node(NUMBER, node->right->number, node->var, NOTHING, NULL, NULL);

                    struct Node* leftNode = create_node(OPERATOR, node->number, node->var, MUL, difLeftNode, difRightNode);
                    struct Node* rightNode = Diff(arg);

                    result->left = leftNode;
                    result->right = rightNode;

                    return result;
                    break;
                }

                default: {
                    struct Node* result = (struct Node*)calloc(1, sizeof(Node));
                    result->type = OPERATOR;
                    result->op = MUL;

                    struct Node* leftArg = dup_node(node->left);
                    struct Node* rightArg = dup_node(node->right);

                    result->left = create_node(OPERATOR, node->number, node->var, POW, leftArg, rightArg);
                    struct Node* lnRightArg = create_node(FUNCTION, node->number, node->var, LN, dup_node(rightArg), NULL);
                    struct Node* rightNode = create_node(OPERATOR, node->number, node->var, MUL, dup_node(leftArg), lnRightArg);
                    result->right = Diff(rightNode); 

                    free_tree(rightNode);
                    return result;
                    break;
                }
            }

        } default: 
            break;
    }

    case FUNCTION:
        switch (node->op) {
            case SIN: {
                struct Node* result = (struct Node*)calloc(1, sizeof(Node));

                result->type = OPERATOR;
                result->op = MUL;

                struct Node* arg = dup_node(node->left);
                result->left = create_node(FUNCTION, node->number, node->var, COS, arg, NULL);

                result->right = Diff(arg); //mb dup
                
                return result;
                break;
            }

            case COS: {
                struct Node* result = (struct Node*)calloc(1, sizeof(Node));

                result->type = OPERATOR;
                result->op = MUL;

                result->left = create_node(NUMBER, -1.0, node->var, NOTHING, NULL, NULL);
                struct Node* arg = dup_node(node->left);
                result->right = create_node(OPERATOR, node->number, node->var, MUL, create_node(FUNCTION, node->number, node->var, SIN, arg, NULL), Diff(arg));

                return result;
                break;
            }

            case TG: {
                struct Node* tmp = (struct Node*)calloc(1, sizeof(Node));

                tmp->type = OPERATOR;
                tmp->op = DIV;

                struct Node* arg = dup_node(node->left);

                tmp->left = create_node(FUNCTION, node->number, node->var, SIN, arg, NULL);
                tmp->right = create_node(FUNCTION, node->number, node->var, COS, dup_node(arg), NULL); //mb dup arg

                struct Node* result = Diff(tmp);

                free_tree(tmp);
                return result;
                break;
            }

            case CTG: {
                struct Node* tmp = (struct Node*)calloc(1, sizeof(Node));

                tmp->type = OPERATOR;
                tmp->op = DIV;

                struct Node* arg = dup_node(node->left);

                tmp->left = create_node(FUNCTION, node->number, node->var, COS, arg, NULL);
                tmp->right = create_node(FUNCTION, node->number, node->var, SIN, dup_node(arg), NULL); //mb dup arg

                struct Node* result = Diff(tmp);

                free_tree(tmp);
                return result;
                break;
            }

            case LN: {
                struct Node* arg = dup_node(node->left);

                struct Node* result = create_node(OPERATOR, node->number, node->var, DIV, Diff(arg), arg);
                return result;
                break;
            }

            default :
                break;

        } default:
            break;
    }

    assert(0);
    return NULL;
}

void free_tree(struct Node* node) {
    if (node == NULL)
        return;

    free_tree(node->left);
    free_tree(node->right);

    free(node);
    node = NULL; 
    return;
}

static struct Node* Get_G(const char* str) { //TODO make (x+1)(x+2) mb here
    s = str;

    struct Node* node = Get_E();

    CHECK_NULL(node, NO_ERRORS, return NULL);

    if (*s != '\0') {
        PRINT_ERROR(NOT_FOUND_OPERATOR);
        free_tree(node);
        return NULL;
    }

    return node;
}

static struct Node* Get_E() {
    struct Node* node = NULL; 

    int amount = 0;

    struct Node* leftNode = Get_T();
    CHECK_NULL(leftNode, NO_ERRORS, return NULL);

    while ((*s == '+') || (*s == '-')) {
        if (amount >= 1) {
            operatorType op = get_operator(*s);
            ++s;

            struct Node* rightNode = Get_T();
            CHECK_NULL(rightNode, NO_ERRORS, free_tree(node); return NULL);

            struct Node* newNode = create_node(OPERATOR, 0.0, 0, node->op, node->left, node->right);
            CHECK_NULL(rightNode, NO_ERRORS, free_tree(node); free_tree(rightNode); return NULL);

            node->left = newNode;
            node->right = rightNode;
            node->op = op;

        } else {
            operatorType op = get_operator(*s);
            ++s;

            struct Node* rightNode = Get_T();
            CHECK_NULL(rightNode, NO_ERRORS, free_tree(leftNode); return NULL);

            node = create_node(OPERATOR, 0.0, 0, op, leftNode, rightNode);  
            CHECK_NULL(node, NO_ERRORS, free_tree(leftNode); free_tree(rightNode); return NULL);
        }

        ++amount;
    }

    if (amount == 0)
        return leftNode;

    return node;
}

static struct Node* Get_T() {
    struct Node* node = NULL; 

    int amount = 0;

    struct Node* leftNode = Get_Pow();
    CHECK_NULL(leftNode, NO_ERRORS, return NULL);

    while ((*s == '*') || (*s == '/')) {
        if (amount >= 1) {
            operatorType op = get_operator(*s);
            ++s;

            struct Node* rightNode = Get_Pow();
            CHECK_NULL(rightNode, NO_ERRORS, free_tree(node); return NULL);

            struct Node* newNode = create_node(OPERATOR, 0.0, 0, node->op, node->left, node->right);
            CHECK_NULL(rightNode, NO_ERRORS, free_tree(node); free_tree(rightNode); return NULL);

            node->left = newNode;
            node->right = rightNode;
            node->op = op;

        } else {
            operatorType op = get_operator(*s);
            ++s;

            struct Node* rightNode = Get_Pow();
            CHECK_NULL(rightNode, NO_ERRORS, free_tree(leftNode); return NULL);

            node = create_node(OPERATOR, 0.0, 0, op, leftNode, rightNode);  
            CHECK_NULL(node, NO_ERRORS, free_tree(leftNode); free_tree(rightNode); return NULL);
        }

        ++amount;
    }

    if (amount == 0)
        return leftNode;

    return node;
}

static struct Node* Get_Pow() {
    struct Node* node = NULL;

    int amount = 0;

    struct Node* leftNode = Get_P();
    CHECK_NULL(leftNode, NO_ERRORS, return NULL);

    while (*s == '^') {
        ++s;

        if (amount >= 1) {
            struct Node* rightNode = Get_P();
            CHECK_NULL(rightNode, NO_ERRORS, free_tree(node); return NULL);

            struct Node* newNode = create_node(OPERATOR, 0.0, 0, POW, node->right, rightNode);
            CHECK_NULL(newNode, NO_ERRORS, free_tree(node); free_tree(rightNode); return NULL);

            node->right = newNode;

        } else {
            struct Node* rightNode = Get_P();
            CHECK_NULL(rightNode, NO_ERRORS, free_tree(leftNode); return NULL);

            node = create_node(OPERATOR, 0.0, 0, POW, leftNode, rightNode);
            CHECK_NULL(node, NO_ERRORS, free_tree(leftNode); free_tree(rightNode); return NULL);
        }

        ++amount;
    } 

    if (amount == 0) 
        return leftNode;

    return node;
}

static struct Node* Get_P() {
    struct Node* node = NULL;

    if (*s == '(') {
        ++s;
        node = Get_E();

        CHECK_NULL(node, NO_ERRORS, return NULL);

        if (*s != ')') {
            PRINT_ERROR(PAIR_BRACKETS);
            free_tree(node);
            return NULL;
        }

        ++s;

    } else {
        node = Get_N();
    }

    return node;
}

static struct Node* Get_N() {
    struct Node* node = NULL;

    double number = 0.0;
    int amountOfSymbols = 0;

    if (sscanf(s, "%lf %n", &number, &amountOfSymbols) > 0) {
        node = create_node(NUMBER, number, 0, NOTHING, NULL, NULL);
        CHECK_NULL(node, NO_ERRORS, return NULL);

        s+= amountOfSymbols;

    } else if ((*s == 'x') || (*s == 'X')) {
        node = create_node(VARIABLE, 0.0, *s, NOTHING, NULL, NULL);
        CHECK_NULL(node, NO_ERRORS, return NULL);

        ++s;

    } else {
        node = get_function_node();
        CHECK_NULL(node, NO_ERRORS, return NULL);
    }

    return node;
}

static struct Node* create_node(const nodeType type, const double number, const char var, const operatorType op, struct Node* left, struct Node* right) {
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

static struct Node* dup_node(struct Node* node) { //TODO check
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

static bool match(const char* str) {
    for (size_t index = 0; index < (sizeof(allowedFunctionNames) / sizeof(allowedFunctionNames[0])); ++index) {
        if (strcmp(str, allowedFunctionNames[index]) == 0)
            return true;
    }

    return false;
}

static char* get_function_name() {
    size_t counter = 0;
    const size_t strLength = strlen(s);

    char* functionName = (char*)calloc(maxLengthOfFunctionNames + 1, sizeof(char));
    CHECK_NULL(functionName, MEM_ERROR, return NULL);

    bool stop = false;

    while ((!stop) && (counter < maxLengthOfFunctionNames) && (counter < strLength)) {
        functionName[counter] = *s;
        ++counter;
        functionName[counter] = '\0';

        if (counter >= 2) //min length of functions (ln)
            stop = match(functionName);

        ++s;
    }

    if (!stop) {
        PRINT_ERROR(FORBIDDEN_SYMBOL);
        free(functionName);
        return NULL; 
    }

    return functionName;
}

static struct Node* get_function_node() {
    char* functionName = get_function_name();
    CHECK_NULL(functionName, NO_ERRORS, return NULL);
    
    if (*s != '(') {
        PRINT_ERROR(NO_BRACKET_IN_FUNCTION);
        free(functionName);
        return NULL;
    }

    ++s;

    struct Node* node = NULL; 
    struct Node* nodeLeft = Get_E();

    CHECK_NULL(nodeLeft, NO_ERRORS, free(functionName); return NULL);

    if (strcmp(functionName, "sin") == 0) {
        node = create_node(FUNCTION, 0.0, 0, SIN, nodeLeft, NULL);

    } else if (strcmp(functionName, "cos") == 0) {
        node = create_node(FUNCTION, 0.0, 0, COS, nodeLeft, NULL);

    } else if (strcmp(functionName, "tg") == 0) {
        node = create_node(FUNCTION, 0.0, 0, TG, nodeLeft, NULL);

    } else if (strcmp(functionName, "ctg") == 0) {
        node = create_node(FUNCTION, 0.0, 0, CTG, nodeLeft, NULL);
    
    } else if (strcmp(functionName, "ln") == 0) {
        node = create_node(FUNCTION, 0.0, 0, LN, nodeLeft, NULL);

    } else {
        //add functions here
    }

    CHECK_NULL(node, NO_ERRORS, free(functionName); free_tree(nodeLeft); return NULL);

    if (*s != ')') {
        PRINT_ERROR(PAIR_BRACKETS);
        free(functionName);
        free_tree(node);
        return NULL;
    }

    ++s;

    free(functionName);
    return node;
}

static void clean_stdinput() {
    int str = getchar();
    while ((str != EOF) && (str != '\n') && (str != '\0'))
        str = getchar();
}

static operatorType get_operator(const int symbol) {
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
    }
}