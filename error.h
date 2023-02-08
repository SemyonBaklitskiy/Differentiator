enum errors {
    NO_ERRORS              = 0,
    NULL_GIVEN             = 1,
    MEM_ERROR              = 2,
    EMPTY_INPUT            = 3,
    NOT_FOUND_OPERATOR     = 4,
    PAIR_BRACKETS          = 5,
    FORBIDDEN_SYMBOL       = 6,
    NO_BRACKET_IN_FUNCTION = 7,
    WRONG_TYPE             = 8,
};

void processor_of_errors(errors error, const char* fileName, const char* functionName, const int line);