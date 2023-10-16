#include "stdio.h"

// Loop invariant code motion simple loop test

int sum_loop(int *data, int val, int len)
{
  int s = 0;
  for (int i = 0; i < len; ++i) {
    int x = val;
    s += data[i];
    s += x;
  }
  return s;
}

int main() {
  int nums[] = { 0, 1, 2, 3 };
  printf("%d\n", sum_loop(nums, 4, 4));
  // 22
}
