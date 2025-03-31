// Options:   --concise --ptr-size 4 --no-checksum --no-safe-math --output src/tests/ins/tcase9.c
#include "csmith.h"

volatile uint64_t csmith_sink_ = 0;

static long __undefined;


struct S0 {
   uint32_t  f0;
   const int16_t  f1;
   uint32_t  f2;
   uint32_t  f3;
};





static struct S0  func_1(void);




static struct S0  func_1(void)
{ 
    struct S0 l_2 = {9UL,8L,5UL,0x7B5CA986L};
    return l_2;
}





int main (int argc, char* argv[])
{
    int print_hash_value = 0;
    if (argc == 2 && strcmp(argv[1], "1") == 0) print_hash_value = 1;
    platform_main_begin();
    func_1();
    platform_main_end(0,0);
    return 0;
}
