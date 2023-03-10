#include <stdio.h>
#include <stdlib.h>
#include "functions.h"

#ifdef DUMP
static const char fileTreeName[] = "tree.gv";
static const char diffTreeFileName[] = "diff_tree.gv";
#endif

int main() {
    char* str = get_str();
    RETURN_IF_NULL(str, return 0);

    struct Node* tree = get_tree(str);
    RETURN_IF_NULL(tree, free(str); return -1);

    struct Node* diffTree = get_diff_tree(tree);
    RETURN_IF_NULL(diffTree, free_tree(tree); free(str); return -1);

    output(diffTree);

#ifdef DUMP
    dump(tree, fileTreeName);
    dump(diffTree, diffTreeFileName);
#endif

    free_memory(tree, diffTree, str);
    return 0;
}