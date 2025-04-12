#include <stdint.h>

unsigned int returned = 0;
int print_hash_value = 0;
void tcrc(uint32_t val) { returned ^= val; }
void crc32_gentab(void) {
  returned = 0;
  return;
}
