#include "my_stdint.h"

int main(int argc, char **argv) {
  int t1 = 0, t2 = 1, nextTerm = 0, n = 100;
  // printf("Enter a positive number: ");
  // scanf("%d", &n);

  // displays the first two terms which is always 0 and 1
  // printf("Fibonacci Series: %d, %d, ", t1, t2);
  nextTerm = t1 + t2;

  while (nextTerm <= n) {
    // printf("%d, ", nextTerm);
    t1 = t2;
    t2 = nextTerm;
    nextTerm = t1 + t2;
  }

  return nextTerm;
}
