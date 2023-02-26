#include "functions.h"

#define create_number(number) create_node(NUMBER, number, 0, NOTHING, NULL, NULL)
#define create_var(var) create_node(VARIABLE, 0.0, var, NOTHING, NULL, NULL)
#define create_operator(op, left, right) create_node(OPERATOR, 0.0, 0, op, left, right)
#define create_function(func, left) create_node(FUNCTION, 0.0, 0, func, left, NULL)

struct Node* create_node(const nodeType type, const double number, const char var, const opAndFuncType op, struct Node* left, struct Node* right);
struct Node* dup_node(const struct Node* node);
void copy_node(struct Node* dest, const struct Node* src);