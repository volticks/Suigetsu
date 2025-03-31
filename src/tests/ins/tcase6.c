// Options:   --concise --ptr-size 4 --no-checksum --no-safe-math --output src/tests/ins/tcase6.c
#include "csmith.h"

volatile uint64_t csmith_sink_ = 0;

static long __undefined;



static int32_t g_3 = 1L;



static const int64_t  func_1(void);




static const int64_t  func_1(void)
{ 
    int32_t *l_2 = &g_3;
    int32_t *l_4 = (void*)0;
    int32_t *l_5 = &g_3;
    int32_t *l_6 = &g_3;
    int32_t *l_7 = (void*)0;
    int32_t *l_8[1];
    uint64_t l_9 = 0xA5223FA3A92BD42CLL;
    int i;
    for (i = 0; i < 1; i++)
        l_8[i] = &g_3;
    --l_9;
    return g_3;
}





int main (int argc, char* argv[])
{
    int print_hash_value = 0;
    if (argc == 2 && strcmp(argv[1], "1") == 0) print_hash_value = 1;
    platform_main_begin();
    func_1();
    csmith_sink_ = g_3;
    platform_main_end(0,0);
    return 0;
}
