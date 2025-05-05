#include "my_stdint.h"
size_t strlen(const char *);

int main() {
  char *p;

  p = "hello";
  return strlen(p) - 5;
}
