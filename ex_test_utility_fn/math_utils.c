#include "math_utils.h"

int *fibb(int *fibo) {
    int i;
    fibo[0] = 0;
    fibo[1] = 1;
    for (i = 2; i <= 9; i++) {
        fibo[i] = fibo[i - 1] + fibo[i - 2];
    }
    return fibo;
}

int add(int a, int b) { return a + b; }
