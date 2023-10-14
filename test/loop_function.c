#include "stdio.h"

// Loop invariant code motion simple loop test

// simple function indirection
float sq(float a) {
  return a * a;
}

float sum_plus_square_function(int *data, int len) {
  float s = 0;
  float a = 9267039.0;
  float b = 8578933.0;
  for (int i = 0; i < len; ++i) {
    float c = a * b;   // LICM should move out
    float d = sq(c);   // LICM should move out
    s += d;
  }
  return s;
}

// prints
// 126409118260489716659191283712.000000

int main() {
  int nums[] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
  printf("%5f\n", sum_plus_square_function(nums, 20));
}
