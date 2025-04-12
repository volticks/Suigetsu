
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

static struct S1 func_1(uint32_t p_6) {
  int32_t l_1297 = (-1L);
  int32_t l_1298 = 0xA9CF9E93L;
  int32_t *l_1300 = &l_1297;
  int32_t **l_1299 = &l_1300;
  struct S2 l_1304 = {-5603, 3};
  struct S2 *l_1303 = &l_1304;
  struct S2 **l_1302 = &l_1303;
  struct S2 ***l_1301 = &l_1302;
  struct S2 ****l_1305[8] = {&l_1301, &l_1301, &l_1301, &l_1301,
                             &l_1301, &l_1301, &l_1301, &l_1301};
  struct S2 ***l_1306[1][7][1] = {{{(void *)0},
                                   {&l_1302},
                                   {(void *)0},
                                   {&l_1302},
                                   {(void *)0},
                                   {&l_1302},
                                   {(void *)0}}};
  struct S1 l_1307 = {1300};
  int i, j, k;
  l_1297 = (l_1298 ^= (l_1297 != l_1297));
  (*l_1299) = &l_1297;
  (*l_1300) = ((p_6, (*l_1300)) <= ((l_1306[0][2][0] = l_1301) == (void *)0));
  tcrc(*l_1300);
  tcrc(**l_1299);
  tcrc(l_1307.f0);
  tcrc(l_1304.f1);
  return l_1307;
}

int main(int argc, char *argv[]) {
  int print_hash_value = 0;
  crc32_gentab();
  func_1(0x123456);
  return returned;
}
