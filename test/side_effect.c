#include <stdio.h>

// LICM side effect test

void incr(int *x) {
    *x += 10;
}

int main() {
    int a = 5;

    for (int i = 0; i < 10; i++) {
        incr(&a); // do not hoist
    }

    printf("%d\n", a);
    return 0;
    // 105
}

