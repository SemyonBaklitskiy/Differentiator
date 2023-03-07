#pragma once

enum errors {
    NO_ERRORS                = 0,
    NULL_GIVEN               = 1,
    MEM_ERROR                = 2,
    EMPTY_INPUT              = 3,
    NOT_FOUND_OPERATOR       = 4,
    PAIR_BRACKETS            = 5,
    FORBIDDEN_SYMBOL         = 6,
    NO_BRACKET_IN_FUNCTION   = 7,
    WRONG_TYPE_OR_OP_OR_FUNC = 8,
    OUTPUT_ERROR             = 9,
    FILE_ERROR               = 10,
};

#define PRINT_ERROR(error) processor_of_errors(error, __FILE__, __PRETTY_FUNCTION__, __LINE__)
#define CHECK_NULL(pointer, error, ...) if (pointer == NULL) { PRINT_ERROR(error); __VA_ARGS__; }

void processor_of_errors(errors error, const char* fileName, const char* functionName, const int line);