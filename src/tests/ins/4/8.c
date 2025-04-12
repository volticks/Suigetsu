
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

static struct S3 *func_1(const int16_t **const p_67, uint64_t p_68,
                         uint32_t p_69) {
  int32_t *l_126[2];
  int8_t l_127[9][3][9] = {
      {{1L, 7L, 3L, (-2L), 0xE3L, 0x29L, (-1L), 2L, 9L},
       {0L, 1L, 0x4AL, 0x1FL, 0xC7L, 0xC4L, 0L, 1L, 0x63L},
       {(-6L), 0xA6L, 0xE0L, (-7L), 0L, 0xC7L, 0xEEL, 0x75L, 0x78L}},
      {{(-10L), (-1L), 0x29L, (-1L), (-6L), 0L, 0x94L, 5L, 0x36L},
       {0xE3L, 0x29L, 9L, (-6L), 0x20L, (-1L), (-7L), 0x1DL, 1L},
       {(-10L), (-6L), 1L, 0xB9L, (-9L), 0xB9L, 1L, (-6L), (-10L)}},
      {{0L, 0x78L, 0xB9L, 0x29L, 0xB2L, (-2L), 0x62L, 0xE3L, (-9L)},
       {3L, 0x20L, (-6L), 8L, (-10L), 0xC7L, 0x75L, 2L, 0x84L},
       {0L, 0xB2L, 0x62L, 7L, 1L, 0x1DL, 3L, 0x8CL, 4L}},
      {{(-10L), 0xA6L, (-6L), 0x73L, 4L, 0x94L, (-3L), 4L, 2L},
       {0xE3L, 0L, 0L, 0x62L, (-1L), 4L, (-7L), 0x4CL, (-6L)},
       {0x1FL, 0xE0L, 0L, 1L, (-1L), 1L, 4L, 0x63L, 0xC7L}},
      {{0x4EL, 0x6BL, (-6L), 0x63L, 0x78L, 7L, 4L, 7L, 0x78L},
       {0xEEL, 0x62L, 0x62L, 0xEEL, 0x8CL, 0xD2L, 1L, (-9L), (-6L)},
       {0xC7L, (-6L), (-6L), 0x36L, 2L, 0xE3L, 0xB9L, 1L, (-7L)}},
      {{5L, (-7L), 0xB9L, 9L, 0x8CL, (-9L), 0x9AL, 0x17L, 0x75L},
       {(-3L), (-10L), 1L, 0x1FL, 0x78L, (-2L), 0x84L, 0x29L, 0x29L},
       {0x6BL, (-7L), 9L, 2L, (-1L), 0x29L, 0xE3L, (-2L), 3L}},
      {{1L, 0x29L, (-7L), (-1L), (-1L), 0x84L, 0xE3L, 0xEEL, 0x4EL},
       {(-1L), 0x8CL, (-2L), (-3L), 4L, (-6L), 0x84L, 0L, 7L},
       {(-1L), 0x9AL, 0L, 1L, 1L, 0L, 0x9AL, (-1L), 2L}},
      {{1L, 0x36L, 0xC4L, 0x29L, (-10L), 3L, 0xB9L, (-10L), 0x1FL},
       {0L, 5L, 0x1DL, 0x78L, 0xB2L, 0x29L, 1L, (-1L), 2L},
       {(-1L), 2L, 0xB2L, 2L, (-9L), 0x4EL, 4L, 0x9AL, 7L}},
      {{0xB9L, 1L, 0xC7L, (-2L), 0x20L, 0x62L, 8L, 0x63L, 1L},
       {0L, 5L, 1L, 0xE3L, 0x4EL, (-6L), (-1L), 0x73L, 0xC4L},
       {0L, 5L, 0x75L, 0x63L, (-1L), 0xA6L, 2L, 0x4EL, 0x4CL}}};
  int32_t l_128 = 2L;
  int32_t l_129 = 0x3C2908FFL;
  int64_t l_130 = 0L;
  uint16_t l_131 = 5UL;
  uint64_t l_141 = 9UL;
  uint64_t *l_140 = &l_141;
  int8_t *l_145[8][9];
  int8_t **l_144[9];
  const int8_t l_148[8] = {(-5L), (-5L), (-5L), (-5L),
                           (-5L), (-5L), (-5L), (-5L)};
  const int8_t *l_147 = &l_148[5];
  const int8_t **l_146 = &l_147;
  uint32_t l_149 = 0x7C073804L;
  uint16_t l_150 = 0UL;
  uint32_t l_160 = 2UL;
  uint32_t *l_159[5] = {&l_160, &l_160, &l_160, &l_160, &l_160};
  int8_t l_161[6][6] = {{0xABL, 0x78L, 0xABL, 0x78L, 0xABL, 0x78L},
                        {0x2CL, 0x78L, 0x2CL, 0x78L, 0x2CL, 0x78L},
                        {0xABL, 0x78L, 0xABL, 0x78L, 0xABL, 0x78L},
                        {0x2CL, 0x78L, 0x2CL, 0x78L, 0x2CL, 0x78L},
                        {0xABL, 0x78L, 0xABL, 0x78L, 0xABL, 0x78L},
                        {0x2CL, 0x78L, 0x2CL, 0x78L, 0x2CL, 0x78L}};
  uint8_t l_169 = 255UL;
  uint8_t *l_168 = &l_169;
  uint8_t l_170 = 1UL;
  int32_t l_171 = 0x398A9316L;
  int64_t l_172 = 0xBCDC44B7573974BALL;
  struct S3 l_174 = {0xFBC400501E52DC8ALL,
                     0x4C2F740028899DD6LL,
                     65535UL,
                     0x92L,
                     0xB74AA1CCE248CDE0LL,
                     0L,
                     {0xCB42L, 66, 0x90B6805CL, 0x9FL, 0xC563E0C8L,
                      0xFF6DB796997B7948LL, 23749, 0UL}};
  struct S3 *l_173[5];
  struct S3 *l_175 = &l_174;
  struct S3 *l_176 = (void *)0;
  int i, j, k;
  for (i = 0; i < 2; i++)
    l_126[i] = (void *)0;
  for (i = 0; i < 8; i++) {
    for (j = 0; j < 9; j++)
      l_145[i][j] = &l_127[7][0][3];
    tcrc(*l_145[i][j - 1]); // <---
  }
  for (i = 0; i < 9; i++) {
    l_144[i] = &l_145[1][4];
    tcrc(**l_144[i]);
  }
  for (i = 0; i < 5; i++)
    l_173[i] = &l_174;
  --l_131;
  l_171 |=
      ((int8_t)(((uint16_t)((l_150 =
                                 ((int32_t)(((*l_140)--),
                                            (l_149 |=
                                             (((*l_146) = &l_127[5][1][8]) ==
                                              (void *)0))) >>
                                  (int32_t)12)) <
                            (((l_170 ^=
                               (((*l_168) |=
                                 ((p_69 <=
                                   ((int16_t)(0x9CL <=
                                              (18446744073709551612UL <=
                                               ((uint64_t)((uint32_t)(l_161[5][3] =
                                                                          ((uint64_t)(1UL <
                                                                                      (-3L))
                                                                           << (uint64_t)45)) *
                                                           (uint32_t)p_69) -
                                                (uint64_t)(+(
                                                    ((int64_t)((!((int8_t)((*p_67) !=
                                                                           (void
                                                                                *)0) >>
                                                                  (int8_t)4)) |
                                                               p_68)
                                                     << (int64_t)54) &&
                                                    p_68))))) %
                                    (int16_t)8L)) == 0xD5CE32BEL)) < p_69)) &
                              p_69) <= p_69)) >>
                 (uint16_t)11) > p_68) *
       (int8_t)p_69);
  l_172 = 0x0A026D0CL;
  tcrc(l_171);
  tcrc(l_172);
  return l_176;
}

int main(int argc, char *argv[]) {
  int print_hash_value = 0;
  crc32_gentab();
  int16_t asd = 0x4142;
  int16_t *asdp = &asd;
  func_1(&asdp, 0x41424242343, 0x432113);
  return returned;
}
