#include "../include/math_utils.h"

int add(int a, int b) { return a + b; }

int multiply(int a, int b) { return a * b; }

int divide(int a, int b) {
    if (b == 0) return 0;
    return a / b;
}

int factorial(int n) {
    if (n <= 1) return 1;
    return n * factorial(n - 1);
}
