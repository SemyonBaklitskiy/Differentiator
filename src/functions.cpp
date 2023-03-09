#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "functions.h"
#include "error.h"
#include "build_tree.h"
#include "output_tree.h"
#include "differentiate_tree.h"
#include "math_functions.h"

static void clean_stdinput();

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

void output(const struct Node* diffTree) {
    CHECK_NULL(diffTree, NULL_GIVEN, return);

    printf("f`(x) = ");
    print_tree(diffTree);
    printf("\n");

    printf("Would you like to get LaTex file (Y/N)?: ");
    char answer = 0;
    clean_stdinput();
    scanf("%c", &answer);

    if ((answer == 'Y') || (answer == 'y')) {
        const char fileName[] = "main.tex";
        FILE* stream = fopen(fileName, "w");
        CHECK_NULL(stream, FILE_ERROR, return);

        fprintf(stream, "\\documentclass{letter}\n\\begin{document}\n$f`(x) = ");
        tex_output(diffTree, stream);
        fprintf(stream, "$\n\\end{document}");

        fclose(stream);
    }

}

void dump(const struct Node* tree, const char* fileName) {
    CHECK_NULL(tree, NULL_GIVEN, return);
    dump_tree(tree, fileName);
}

struct Node* get_diff_tree(const struct Node* tree) { 
    CHECK_NULL(tree, NULL_GIVEN, return NULL);
    struct Node* result = differentiate(tree);
    simplify_constants(result);

    return result;
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

void free_memory(struct Node* tree, struct Node* diffTree, char* str) {
    free_tree(diffTree);
    free_tree(tree);
    free(str);

    tree     = NULL;
    diffTree = NULL;
    str      = NULL;
}

static void clean_stdinput() {
    int str = getchar();
    while ((str != EOF) && (str != '\n') && (str != '\0'))
        str = getchar();
}

bool compare(const double firstNumber, const double secondNumber) {
    const double epsilon = 0.00001;
    return (get_fabs(firstNumber - secondNumber) <= epsilon);
}