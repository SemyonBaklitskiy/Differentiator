#include <math.h>
#include <string.h>
#include "math_functions.h"

double get_e() {
    return exp(1);
}

double get_pi() {
    return acos(-1);
}

double get_fabs(const double val) {
    return fabs(val);
}

double get_function(const char* str, const double val) {
    #define generator(function, diffRules, c_funcName) if (strcmp(str, #c_funcName) == 0) return c_funcName(val);
    #include "define.h"
    #undef generator

    return NAN;
}