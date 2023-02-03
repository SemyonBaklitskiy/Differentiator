#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>
#include "functions.h"

static const char* s = NULL;
static const char* names[] = {"sin", "cos", "tg", "ctg", "ln"};

static bool match(const char* str);
void get_node(const char* functionName, struct Node* node);
static struct Node* Get_G(const char* str);
static struct Node* Get_E();
static struct Node* Get_T();
static struct Node* Get_Pow();
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
    if (node == NULL) {
        printf(")");
        return;
    }

    if (node->type == TRIGONOMETRY_OR_LOG) {
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

        case POWER:
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

        case POWER:
            printf("^");
            break;

        default:
            break;
        }
    }

    if ((node->type != TRIGONOMETRY_OR_LOG) && (node->right->type == OPERATOR)) {
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

        case POWER: {
            switch (node->right->type) {
                case NUMBER: {
                    struct Node* result = (struct Node*)calloc(1, sizeof(Node));
                    result->type = OPERATOR;
                    result->op = MUL;

                    struct Node* arg = dup_node(node->left);

                    struct Node* difRightNode = (struct Node*)calloc(1, sizeof(Node));
                    difRightNode->type = OPERATOR;
                    difRightNode->op = POWER;
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

                    result->left = create_node(OPERATOR, node->number, node->var, POWER, leftArg, rightArg);
                    struct Node* lnRightArg = create_node(TRIGONOMETRY_OR_LOG, node->number, node->var, LN, dup_node(rightArg), NULL);
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

    case TRIGONOMETRY_OR_LOG:
        switch (node->op) {
            case SIN: {
                struct Node* result = (struct Node*)calloc(1, sizeof(Node));

                result->type = OPERATOR;
                result->op = MUL;

                struct Node* arg = dup_node(node->left);
                result->left = create_node(TRIGONOMETRY_OR_LOG, node->number, node->var, COS, arg, NULL);

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
                result->right = create_node(OPERATOR, node->number, node->var, MUL, create_node(TRIGONOMETRY_OR_LOG, node->number, node->var, SIN, arg, NULL), Diff(arg));

                return result;
                break;
            }

            case TG: {
                struct Node* tmp = (struct Node*)calloc(1, sizeof(Node));

                tmp->type = OPERATOR;
                tmp->op = DIV;

                struct Node* arg = dup_node(node->left);

                tmp->left = create_node(TRIGONOMETRY_OR_LOG, node->number, node->var, SIN, arg, NULL);
                tmp->right = create_node(TRIGONOMETRY_OR_LOG, node->number, node->var, COS, dup_node(arg), NULL); //mb dup arg

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

                tmp->left = create_node(TRIGONOMETRY_OR_LOG, node->number, node->var, COS, arg, NULL);
                tmp->right = create_node(TRIGONOMETRY_OR_LOG, node->number, node->var, SIN, dup_node(arg), NULL); //mb dup arg

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

    struct Node* nodeLeft = Get_Pow();

    while ((*s == '*') || (*s == '/')) {
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
            struct Node* nodeRight = Get_Pow();

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
            struct Node* nodeRight = Get_Pow();

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

static struct Node* Get_Pow() {
    struct Node* node = NULL;

    bool hasPower = false;
    int amount = 0;

    struct Node* nodeLeft = Get_P();

    while (*s == '^') {
        hasPower = true;
        ++s;

        if (amount >= 1) {
            struct Node* newNode = (struct Node*)calloc(1, sizeof(Node));
            newNode->type = OPERATOR;
            newNode->op = POWER;
            newNode->left = node->right;
            newNode->right = Get_P();

            node->right = newNode;

        } else {
            node = (struct Node*)calloc(1, sizeof(Node));
            node->left = nodeLeft;
            node->right = Get_P();
            node->op = POWER;
            node->type = OPERATOR;
        }

        ++amount;
    } 

    if (!hasPower) 
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

    } else if ((*s == 'x') || (*s == 'X')) {
        node->left = NULL;
        node->right = NULL;
        node->type = VARIABLE;
        node->var = *s;

        ++s;

    } else if (*s == ' ') {
        ++s;

    } else {
        int counter = 0;
        char word[4] = " ";
        bool stop = false;

        while (!stop && counter + 1 < 4) {
            word[counter] = *s;
            ++counter;
            word[counter] = '\0';

            if (counter >= 2) {
                stop = match(word);
            }

            ++s;
        }

        if (!stop) 
            assert(0);
        
        if (*s != '(')
            assert(0);

        ++s;

        get_node(word, node);

        if (*s != ')')
            assert(0);

        ++s;

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

    case TRIGONOMETRY_OR_LOG:
        node->type = TRIGONOMETRY_OR_LOG;
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

static bool match(const char* str) {
    for (size_t index = 0; index < (sizeof(names) / sizeof(names[0])); ++index) {
        if (strcmp(str, names[index]) == 0)
            return true;
    }

    return false;
}

void get_node(const char* functionName, struct Node* node) {
    if (strcmp(functionName, "sin") == 0) {
        node->type = TRIGONOMETRY_OR_LOG;
        node->op = SIN;
        node->right = NULL;

        node->left = Get_E(); //mb get_p

    } else if (strcmp(functionName, "cos") == 0) {
        node->type = TRIGONOMETRY_OR_LOG;
        node->op = COS;
        node->right = NULL;

        node->left = Get_E();

    } else if (strcmp(functionName, "tg") == 0) {
        node->type = TRIGONOMETRY_OR_LOG;
        node->op = TG;
        node->right = NULL;

        node->left = Get_E();

    } else if (strcmp(functionName, "ctg") == 0) {
        node->type = TRIGONOMETRY_OR_LOG;
        node->op = CTG;
        node->right = NULL;

        node->left = Get_E();
    
    } else if (strcmp(functionName, "ln") == 0) {
        node->type = TRIGONOMETRY_OR_LOG;
        node->op = LN;
        node->right = NULL;

        node->left = Get_E();

    } else {

    }
}