#include <stdio.h>
#include "functions.h"

int main() {
    char* str = get_str();
    struct Node* tree = get_tree(str);
    print_tree(tree);
    printf("\n");

    print_tree(Diff(tree));
    printf("\n");

    return 0;
}