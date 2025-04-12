
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
#pragma pack(push)
#pragma pack(1)
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
#pragma pack(pop)

struct S3 {
  int64_t f0;
  uint64_t f1;
  uint16_t f2;
  const uint8_t f3;
  uint64_t f4;
  int32_t f5;
  struct S0 f6;
};

static int32_t func_1(uint16_t *p_83, int16_t **p_84, uint32_t p_85,
                      int16_t **p_86, struct S3 *const p_87) {
  int8_t l_99[6][6][4] = {{{0L, 0xA9L, 0x80L, 0L},
                           {0xA9L, 1L, 8L, 0xA9L},
                           {0xEBL, 6L, 8L, (-1L)},
                           {0xA9L, 0x40L, 0x80L, 0L},
                           {0L, 8L, 5L, (-1L)},
                           {5L, (-1L), (-1L), (-1L)}},
                          {{0L, 0L, 0L, 0x9AL},
                           {6L, 1L, (-1L), 0x37L},
                           {5L, 0x9AL, 0xC9L, (-1L)},
                           {(-1L), 0x9AL, 0x80L, 0x37L},
                           {0x9AL, 1L, (-5L), 0x9AL},
                           {0xEBL, 0L, 1L, (-1L)}},
                          {{1L, (-1L), 0x80L, (-1L)},
                           {0x40L, 8L, 0xEBL, 0L},
                           {5L, 0x40L, 0x0EL, (-1L)},
                           {0x37L, 6L, 0L, 0xA9L},
                           {0x37L, 1L, 0x0EL, 0L},
                           {5L, 0xA9L, 0xEBL, (-1L)}},
                          {{0x40L, 1L, 0x80L, 6L},
                           {1L, 1L, 1L, 1L},
                           {0xEBL, 0x37L, (-5L), (-1L)},
                           {0x9AL, 0L, 0x80L, 0x40L},
                           {(-1L), 8L, 0xC9L, 0x40L},
                           {5L, 0L, (-1L), (-1L)}},
                          {{6L, 0x37L, 0L, 1L},
                           {0L, 1L, (-1L), 6L},
                           {5L, 1L, 5L, (-1L)},
                           {0L, 0xA9L, 0x80L, 0L},
                           {0xA9L, 1L, 8L, 0xA9L},
                           {0xEBL, 6L, 8L, (-1L)}},
                          {{8L, (-1L), 0x37L, (-1L)},
                           {(-1L), 0x80L, 8L, 0x0EL},
                           {8L, 0x0EL, 0L, 0L},
                           {0xC9L, 0xC9L, 6L, 1L},
                           {0xEBL, (-8L), 0L, 5L},
                           {8L, 1L, 0L, 0L}}};
  int8_t *l_98[2][4] = {
      {&l_99[3][4][0], &l_99[3][4][0], &l_99[3][4][0], &l_99[3][4][0]},
      {&l_99[3][4][0], &l_99[3][4][0], &l_99[3][4][0], &l_99[3][4][0]}};
  int32_t l_100 = (-5L);
  int64_t l_104 = 0x2B061077F729EE1FLL;
  int64_t *l_103 = &l_104;
  uint16_t l_108 = 0xC886L;
  uint16_t *l_107[6] = {&l_108, &l_108, &l_108, &l_108, &l_108, &l_108};
  tcrc(*l_107[0]);
  int32_t l_109 = 0L;
  uint64_t *l_110 = (void *)0;
  uint64_t *l_111 = (void *)0;
  uint64_t l_113 = 1UL;
  uint64_t *l_112 = &l_113;
  int32_t l_114 = 0L;
  int64_t *l_115 = (void *)0;
  int64_t *l_116 = (void *)0;
  int64_t l_118 = 0xFA1A8EB3B5FD3493LL;
  int64_t *l_117 = &l_118;
  int32_t l_120 = 0x7298EAA4L;
  int32_t *l_119 = &l_120;
  int32_t l_121 = 0x512B1E0EL;
  int i, j, k;
  (*l_119) =
      (((p_85 ||
         ((*l_117) &=
          ((int8_t)(l_100 = ((int32_t)p_85 >> (int32_t)28)) /
           (int8_t)((int8_t)(p_85 & ((**p_84), ((*l_103) = l_99[2][5][2])))
                    << (int8_t)(l_114 =
                                    (((*l_112) =
                                          (l_109 =
                                               ((((((uint8_t)l_99[3][2][2] -
                                                    (uint8_t)(l_99[3][2][2] ^
                                                              l_99[0][3][1])),
                                                   p_83) != l_107[2]),
                                                 l_99[3][2][2]),
                                                0x76D6F9774F07DEA5LL))),
                                     l_113)))))) != l_99[4][2][0]) &
       l_99[3][2][2]);
  tcrc(*l_119);
  tcrc(l_121);
  return l_121;
}

int main(int argc, char *argv[]) {
  int print_hash_value = 0;
  crc32_gentab();
  uint16_t asd = 0x3333;
  uint16_t *asdp = &asd;
  int16_t **asdpp = &asdp;
  struct S3 asd3 = {0};
  func_1(asdp, asdpp, 0x41424344, asdpp, &asd3);
  return returned;
}
