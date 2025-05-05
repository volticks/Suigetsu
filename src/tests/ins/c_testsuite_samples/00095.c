#include "my_stdint.h"
int x;
int x = 3;
int x;

int main(int argc, char **argv);

void *foo() { return &main; }

int main(int argc, char **argv) {
  if (x != 3)
    return 0;

  x = 0;
  return x;
}
