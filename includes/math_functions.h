enum functions {
    #define generator(function, diffRules, c_funcName) _##c_funcName##_,
    #include "define.h" //code generation for enum
    #undef generator
};

double get_e();
double get_pi();
double get_fabs(const double val);
double get_function(const functions functionType, const double val);

const double e  = get_e();
const double pi = get_pi();
