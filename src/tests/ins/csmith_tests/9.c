#ifndef __MN10300__
#include "../ntv_includes.h"
#else
#include "../my_stdint.h"
#endif

union U4 {
  uint32_t f0;
};
#pragma pack(push)
#pragma pack(1)
struct S1 {
  unsigned f0 : 21;
};
#pragma pack(pop)

#pragma pack(push)
#pragma pack(1)
struct S2 {
  const signed f0 : 28;
  unsigned f1 : 6;
};
#pragma pack(pop)

static uint64_t func_1(int8_t p_73, uint16_t *const p_74, uint16_t p_75) {
  uint16_t *l_123 = (void *)0;
  tcrc((uint32_t)(l_123));
  uint16_t **l_122 = &l_123;
  uint16_t ***l_124 = &l_122;
  (*l_124) = l_122;
  tcrc(p_73);
  tcrc(*p_74);
  tcrc(p_75);
  return p_73;
}

int main(int argc, char *argv[]) {
  int print_hash_value = 0;
  crc32_gentab();
  uint16_t asd = 0x4444;
  func_1(0x12, &asd, 0x3333);
  return returned;
}
