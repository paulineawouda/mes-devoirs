#include "test_func.h"
#include "fast_trigo.h"
#include <stdio.h>

void test_trigo_functions(void) {
    double result = fast_sin(1.0);
    printf("fast_sin(1.0) = %f\n", result);
    result = fast_cos(1.0);
    printf("fast_cos(1.0) = %f\n", result);
}
