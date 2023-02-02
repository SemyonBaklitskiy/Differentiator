#include <stdio.h>
#include <stdlib.h>
#include "functions.h"

int main() {
    char* str = get_str();
    struct Node* tree = get_tree(str);
    print_tree(tree);
    printf("\n");

    struct Node* diffTree = Diff(tree);

    print_tree(diffTree);
    printf("\n");

    free_tree(diffTree);
    free_tree(tree);
    free(str);

    return 0;
}