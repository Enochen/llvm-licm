#include <stdio.h>

// run some numerical calucations to test licm on nested loops

int main() {
    unsigned long computed = 0;
    float samples[9];
    int sampleIdx = 0;
    for (float i_start = 1.0f; i_start <= 10.0f; i_start++) {
        for (int a = -5; a <= 5; a++) {
            for (int b = 1; b <= 5; b++) {
                float i_next = i_start;
                float invariant_val = (float)a / b;
                for (int step = 0; step < 100; step++) {
                    i_next += invariant_val;
                    computed += 1;
                    if (sampleIdx < 9 && computed % (11000/9) == 0) {
                        samples[sampleIdx++] = i_next;
                    }
                }
            }
        }
    }
    // Print the 9 sample outputs
    for (int i = 0; i < 9; i++) {
        printf("%.3f ", samples[i]);
    }
    printf("%lu\n", computed);
    return 0;
    // expected
    // -21.000 -7.800 67.000 89.000 -18.000 -14.000 56.000 78.000 100.000 55000
}

