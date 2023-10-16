#include "stdio.h"

// Loop invariant code motion simple loop test

int sum_plus_tripled_val(int *data, int val, int len)
{
  int s = 0;
  for (int i = 0; i < len; ++i) {
    int x = val * 3;   // LICM should move out
    s += data[i];
    s += x;
  }
  return s;
}

int main() {
  int nums[] = { 0, 0, 0, 0 };
  printf("%d\n", sum_plus_tripled_val(nums, 2, 4));
}
