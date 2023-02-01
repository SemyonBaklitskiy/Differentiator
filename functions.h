enum nodeType {
    VARIABLE = 1,
    NUMBER   = 2,
    OPERATOR = 3,
};

enum operatorType {
    ADD  = 1,
    SUB  = 2,
    MUL  = 3,
    DIV  = 4,
};

struct Node {
    nodeType type;

    int number;
    operatorType op;
    char var;
    
    struct Node* left;
    struct Node* right;
};

char* get_str();
struct Node* get_tree(const char* str);
void print_tree(struct Node* node);