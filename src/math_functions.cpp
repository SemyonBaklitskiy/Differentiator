#include <math.h>
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

double get_function(const functions functionType, const double val) {
    switch (functionType) {

    #define generator(function, diffRules, c_funcName) case _##c_funcName##_: return c_funcName(val);
    #include "define.h" //code generation for get_function
    #undef generator

    default:
        return NAN;
    }
}