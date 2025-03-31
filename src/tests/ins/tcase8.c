// Options:   --concise --ptr-size 4 --no-checksum --no-safe-math --output src/tests/ins/tcase8.c
#include "csmith.h"

volatile uint64_t csmith_sink_ = 0;

static long __undefined;


union U2 {
   uint16_t  f0;
   volatile uint8_t  f1;
   int32_t  f2;
   int32_t  f3;
   uint8_t  f4;
};


static volatile int8_t g_4 = (-4L);
static union U2 g_5 = {1UL};



static union U2  func_1(void);




static union U2  func_1(void)
{ 
    int32_t *l_2 = (void*)0;
    int32_t **l_3 = &l_2;
    (*l_3) = l_2;
    g_4 &= 1L;
    return g_5;
}





int main (int argc, char* argv[])
{
    int print_hash_value = 0;
    if (argc == 2 && strcmp(argv[1], "1") == 0) print_hash_value = 1;
    platform_main_begin();
    func_1();
    csmith_sink_ = g_4;
    csmith_sink_ = g_5.f0;
    platform_main_end(0,0);
    return 0;
}
