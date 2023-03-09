#include "functions.h"

void print_tree(const struct Node* node); 
void tex_output(const struct Node* node, FILE* stream);
void dump_tree(const struct Node* tree, const char* fileName);
bool compare(const double firstNumber, const double secondNumber);