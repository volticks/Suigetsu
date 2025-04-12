

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

static int32_t func_1(uint16_t p_40, int16_t *p_41, int64_t p_42, uint64_t p_43,
                      int16_t p_44) {
  uint8_t l_261 = 0xC6L;
  int32_t l_272 = 0x51F46808L;
  int32_t *l_271 = &l_272;
  int32_t **l_270 = &l_271;
  int32_t **l_273 = (void *)0;
  struct S0 l_283[6] = {{0x5A4DL, 41, 1L, 0L, 0L, 7L, 29881, 0xD969L},
                        {0x5A4DL, 41, 1L, 0L, 0L, 7L, 29881, 0xD969L},
                        {0x5A4DL, 41, 1L, 0L, 0L, 7L, 29881, 0xD969L},
                        {0x5A4DL, 41, 1L, 0L, 0L, 7L, 29881, 0xD969L},
                        {0x5A4DL, 41, 1L, 0L, 0L, 7L, 29881, 0xD969L},
                        {0x5A4DL, 41, 1L, 0L, 0L, 7L, 29881, 0xD969L}};
  struct S0 *l_282 = &l_283[3];
  int32_t l_306 = (-1L);
  int i;
  l_261 = p_44;
  for (p_44 = (-15); (p_44 == 23); p_44 += 5) {
    const uint8_t *l_274 = &l_261;
    int32_t l_279 = (-1L);
    struct S0 *l_284[6][9] = {
        {&l_283[3], &l_283[3], &l_283[3], &l_283[3], &l_283[3], &l_283[3],
         &l_283[3], &l_283[3], &l_283[3]},
        {(void *)0, (void *)0, (void *)0, (void *)0, &l_283[3], (void *)0,
         (void *)0, (void *)0, (void *)0},
        {&l_283[4], &l_283[3], &l_283[2], &l_283[3], &l_283[4], &l_283[4],
         &l_283[3], &l_283[2], &l_283[3]},
        {(void *)0, &l_283[3], &l_283[1], &l_283[1], &l_283[3], (void *)0,
         &l_283[3], &l_283[1], &l_283[1]},
        {&l_283[4], &l_283[4], &l_283[3], &l_283[2], &l_283[3], &l_283[4],
         &l_283[4], &l_283[3], &l_283[2]},
        {(void *)0, &l_283[3], (void *)0, (void *)0, (void *)0, (void *)0,
         &l_283[3], (void *)0, (void *)0}};
    int32_t l_285 = 0x695163A1L;
    int32_t l_286 = 0L;
    int16_t l_292 = 8L;
    int16_t *l_291 = &l_292;
    int8_t **l_293 = (void *)0;
    int16_t l_295 = 0xD457L;
    int16_t *l_294 = &l_295;
    int i, j;
    l_286 &=
        ((int32_t)((((uint8_t)(l_270 != l_273) +
                     (uint8_t)((void *)0 != l_274)) >= 0L) &&
                   ((((uint32_t)(((-10L) != ((uint16_t)(l_279 = (p_40, 0xB4B4L))
                                             << (uint16_t)0)) &&
                                 ((int16_t)(l_282 == l_284[5][3]) *
                                  (int16_t)(**l_270))) -
                      (uint32_t)l_285),
                     p_43) != p_43)) /
         (int32_t)p_42);
    l_285 |=
        (((**l_270) =
              ((int8_t)p_40 -
               (int8_t)(((l_279 =
                              ((*l_294) = ((int16_t)((*l_291) &=
                                                     (l_279 ^ 0xA8402F03L)) *
                                           (int16_t)((void *)0 == l_293)))) &&
                         ((uint8_t)((uint64_t)((*l_282),
                                               (((p_43 ==
                                                  ((~(((+((int8_t)((int16_t)(**l_270)
                                                                   << (int16_t)((void
                                                                                     *)0 ==
                                                                                &p_42)) /
                                                          (int8_t)p_40)) &
                                                       0UL),
                                                      p_42)) |
                                                   0xBCL)) &&
                                                 p_44) ^
                                                l_306))
                                    << (uint64_t)l_286) /
                          (uint8_t)255UL)) &&
                        (-7L)))) < 65535UL);

    tcrc(l_286);
    tcrc(l_285);
    tcrc(p_44);
    tcrc(*p_41);
    tcrc(l_284[0][0]->f0);

    return p_44;
  }
  return p_40;
}

int main(int argc, char *argv[]) {
  int print_hash_value = 0;
  int16_t asd = 0;
  crc32_gentab();
  func_1(0x12, &asd, 0x1234, 0x123456, 0x34);
  return returned;
}
