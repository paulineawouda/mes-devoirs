// No include guards! Will cause multiple inclusion errors.
#include <math.h>

float fast_sin(float x) {  // ERROR: Function defined inside .h
    return sinf(x);
}

float fast_cos(float x) {  // ERROR: Function defined inside .h
    return cosf(x);
}

