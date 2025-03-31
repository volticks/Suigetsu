// Options:   --concise --ptr-size 4 --no-checksum --no-safe-math --output src/tests/ins/tcase5.c
#include "csmith.h"

volatile uint64_t csmith_sink_ = 0;

static long __undefined;



static uint32_t g_2[2] = {0xDA115252L,0xDA115252L};



static uint64_t  func_1(void);




static uint64_t  func_1(void)
{ 
    return g_2[1];
}





int main (int argc, char* argv[])
{
    int i;
    int print_hash_value = 0;
    if (argc == 2 && strcmp(argv[1], "1") == 0) print_hash_value = 1;
    platform_main_begin();
    func_1();
    for (i = 0; i < 2; i++)
    {
        csmith_sink_ = g_2[i];
    }
    platform_main_end(0,0);
    return 0;
}
