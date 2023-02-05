#include <stdio.h>
#include <stdlib.h>
#include "functions.h"

int main() {
    char* str = get_str();
    RETURN_IF_NULL(str, return 0);

    struct Node* tree = get_tree(str);
    RETURN_IF_NULL(tree, free(str); return -1);

    struct Node* diffTree = Diff(tree);

    print_tree(diffTree);
    printf("\n");

    free_tree(diffTree);
    free_tree(tree);
    free(str);

    return 0;
}