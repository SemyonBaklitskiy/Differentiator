generator(sin, operator(MUL, function(cos, dup(Left)), diff(Left)), sin)
generator(cos, operator(MUL, operator(MUL, number(-1.0), function(sin, dup(Left))), diff(Left)), cos)
generator(tan, operator(DIV, diff(Left), operator(POW, function(cos, dup(Left)), number(2.0))), tan)
generator(ln, operator(DIV, diff(Left), dup(Left)), log)
generator(exp, operator(MUL, dup(node), diff(Left)), exp)
generator(sinh, operator(MUL, function(cosh, dup(Left)), diff(Left)), sinh)
generator(cosh, operator(MUL, function(sinh, dup(Left)), diff(Left)), cosh)
generator(tanh, operator(DIV, diff(Left), operator(POW, function(cosh, dup(Left)), number(2.0))), tanh)
generator(sqrt, operator(DIV, diff(Left), operator(MUL, number(2.0), dup(node))), sqrt)

/*
To ddd new functions follow these rules:
generator(LaTex name of function, differentiate rules, name of function in <math.h>)
*/