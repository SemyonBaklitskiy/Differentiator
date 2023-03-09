generator(sin, operator(MUL, function(cos, dup(Left)), diff(Left)), sin)
generator(cos, operator(MUL, operator(MUL, number(-1.0), function(sin, dup(Left))), diff(Left)), cos)
generator(tan, operator(MUL, operator(DIV, number(1.0), operator(POW, function(cos, dup(Left)), number(2.0))), diff(Left)), tan)
generator(ln, operator(MUL, operator(DIV, number(1.0), dup(Left)), diff(Left)), log)

/*
To ddd new functions follow these rules:
generator(LaTex name of function, differentiate rules, name of function in <math.h>)
*/