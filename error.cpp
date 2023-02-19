#include <stdio.h>
#include "error.h"

#define ERROR_LOCATION fileName, functionName, line

void processor_of_errors(errors error, const char* fileName, const char* functionName, const int line) {
    switch (error) {
        case NULL_GIVEN:
            printf("In file %s, in function %s, on the line %d: NULL pointer was given as an parameter\n", ERROR_LOCATION);
            break;

        case MEM_ERROR:
            printf("In file %s, in function %s, on the line %d: can`t select a buffer (calloc returned NULL)\n", ERROR_LOCATION);
            break;

        case NOT_FOUND_OPERATOR:
            printf("Syntax error in input: no operator between expressions.\nIn file %s, in function %s, on the line %d: return NULL called.\n", ERROR_LOCATION);
            break;

        case PAIR_BRACKETS:
            printf("Syntax error in input: violation of paired brackets was found.\nIn file %s, in function %s, on the line %d: return NULL called.\n", ERROR_LOCATION);
            break;

        case FORBIDDEN_SYMBOL:
            printf("Syntax error in input: forbiden symbol was found. Or no expression before/after operator.\nIn file %s, in function %s, on the line %d: return NULL called.\n", ERROR_LOCATION);
            break;

        case NO_BRACKET_IN_FUNCTION:
            printf("Syntax error in input: the argument of the functions must be in the pair brackets.\nIn file %s, in function %s, on the line %d: return NULL called.\n", ERROR_LOCATION);
            break;

        case WRONG_TYPE_OR_OP_OR_FUNC:
            printf("In file %s, in function %s, on the line %d: wrong type or operator or function of node\n", ERROR_LOCATION);
            break;

        default:  //TODO add division by zero
            break;
    }
}