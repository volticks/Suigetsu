
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
struct S0 {
  const int16_t f0;
  signed f1 : 14;
  int32_t f2;
  int8_t f3;
  int32_t f4;
  int64_t f5;
  const unsigned f6 : 31;
  uint16_t f7;
};

struct S3 {
  int64_t f0;
  uint64_t f1;
  uint16_t f2;
  const uint8_t f3;
  uint64_t f4;
  int32_t f5;
  struct S0 f6;
};
static union U4 func_1(int32_t p_9) {
  struct S0 p_11 = {0};
  int16_t l_29 = (-4L);
  int16_t *const l_28 = &l_29;
  struct S1 l_1063 = {197};
  union U4 l_1286[9][3] = {{{0xC7B6BF3DL}, {0xC7B6BF3DL}, {0xC7B6BF3DL}},
                           {{0xC7B6BF3DL}, {0xC7B6BF3DL}, {0xC7B6BF3DL}},
                           {{0xC7B6BF3DL}, {0xC7B6BF3DL}, {0xC7B6BF3DL}},
                           {{0xC7B6BF3DL}, {0xC7B6BF3DL}, {0xC7B6BF3DL}},
                           {{0xC7B6BF3DL}, {0xC7B6BF3DL}, {0xC7B6BF3DL}},
                           {{0xC7B6BF3DL}, {0xC7B6BF3DL}, {0xC7B6BF3DL}},
                           {{0xC7B6BF3DL}, {0xC7B6BF3DL}, {0xC7B6BF3DL}},
                           {{0xC7B6BF3DL}, {0xC7B6BF3DL}, {0xC7B6BF3DL}},
                           {{0xC7B6BF3DL}, {0xC7B6BF3DL}, {0xC7B6BF3DL}}};
  int i, j;
  for (p_11.f4 = 0; (p_11.f4 != 0); p_11.f4 += 7) {
    int16_t l_23 = (-6L);
    int16_t *l_22 = &l_23;
    int16_t *l_30[2][7] = {{&l_29, &l_29, &l_29, &l_29, &l_29, &l_29, &l_29},
                           {&l_29, &l_29, &l_29, &l_29, &l_29, &l_29, &l_29}};
    struct S3 l_31 = {0xB70D8AC507B3925BLL,
                      18446744073709551615UL,
                      65535UL,
                      246UL,
                      18446744073709551606UL,
                      1L,
                      {0xBD3DL, -35, -10L, 0x45L, 0x4502E46AL,
                       0xF4DAD24015E1B460LL, 24305, 0x4BA4L}};
    const int16_t *l_33 = &l_31.f6.f0;
    const int16_t **l_32 = &l_33;
    struct S1 l_1064 = {130};
    int32_t l_1285 = 0xE9F92212L;
    int32_t *l_1284[1][9][4] = {
        {{(void *)0, &l_31.f6.f2, (void *)0, (void *)0},
         {&l_31.f6.f2, &l_31.f6.f2, &l_31.f6.f2, &l_31.f6.f2},
         {&l_31.f6.f2, (void *)0, (void *)0, &l_31.f6.f2},
         {(void *)0, &l_31.f6.f2, (void *)0, (void *)0},
         {&l_31.f6.f2, &l_31.f6.f2, &l_31.f6.f2, &l_31.f6.f2},
         {&l_31.f6.f2, (void *)0, (void *)0, &l_31.f6.f2},
         {(void *)0, &l_31.f6.f2, (void *)0, (void *)0},
         {&l_31.f6.f2, &l_31.f6.f2, &l_31.f6.f2, &l_31.f6.f2},
         {&l_31.f6.f2, (void *)0, (void *)0, &l_31.f6.f2}}};
    int i, j, k;
    p_11.f1 &=
        ((*l_22) = 0xE358L) ||
        (0x77DFL & (*l_30[1][1] = *l_28) || l_31.f0 || ((*l_32) = l_22)) ||
        (((uint8_t)((uint64_t)(((l_1064 = l_1063), l_29) ^ 0x01359A86L) -
                    (uint64_t)l_31.f4) *
          (uint8_t)l_31.f5) == l_29) != l_31.f6.f4;
    tcrc(p_11.f1);
    if (l_29)
      continue;
  }
  tcrc(l_29);
  return l_1286[8][2];
}

int main(int argc, char *argv[]) {
  int print_hash_value = 0;
  crc32_gentab();
  func_1(0x123456);
  return returned;
}
