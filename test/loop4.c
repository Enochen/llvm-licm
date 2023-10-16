#include "stdio.h"

// Loop invariant code motion shadowing and serial dependent loops test

int sum_loop(int *data, int val, int len)
{
  int s = 0;
  int x = 0;
  for (int i = 0; i < len; ++i) {
    int x = val;
    s += data[i];
    s += x;
  }
  for (int i = 0; i < len; ++i) {
    int x = 3;
    s += data[i];
    s += x;
  }
  return s + x;
}

int main() {
  int nums[] = { 0, 1, 2, 3 };
  printf("%d\n", sum_loop(nums, 4, 4));
  // 40
}
