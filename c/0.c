#include <stdio.h>

int compute(int a_0, int n) {
  if (n == 0) {
    return a_0;
  }
  else {
    int a_prev = compute(a_0, n - 1);

    return a_prev / 4 + 7;
  }
}

int main() {
  int ret;

  for (int i = 0; i < 100000000; i++) { // 100 million
    ret = compute(343, 7);
  }

  printf("%d\n", ret);

  return 0;
}
