typedef unsigned long uint64_t;
typedef long int64_t;
typedef unsigned int uint32_t;
typedef int int32_t;
typedef unsigned short uint16_t;
typedef short int16_t;
typedef unsigned char uint8_t;
typedef signed char int8_t;
typedef uint64_t size_t;
// typedef unsigned long uint64_t;
// typedef unsigned long uint64_t;
// typedef unsigned long uint64_t;
// typedef unsigned long uint64_t;
inline void *memcpy(void *dest, const void *src, size_t n) {
  // asm("break\n");
  for (size_t i = 0; i < n; i++) {
    ((char *)dest)[i] = ((char *)src)[i];
  }
  return dest;
}

extern int main(int argc, char **argv);

// This needs to set up the stack aswell before we call into main().
//__attribute__((naked)) void start(void) {
//
//  asm(".global main\n"
//      "mov 0x9000, d0\n"
//      "mov d0, a0\n"
//      "mov a0, sp\n"
//      "movm [a3], (sp)\n"
//      "mov sp, a3\n"
//      "add -12, sp\n"
//      "mov 0, d1\n"
//      "mov 0, d0\n"
//      "call _main,[],0\n"
//      "nop\n"
//      "mov a3, sp\n"
//      "ret [a3], 4\n"
//
//  );
//
//  // int a = 0 + 2;
//  // main(0, 0);
//}

asm(".global _start\n"
    "_start:"
    //"mov 0x9000, d0\n"
    "mov 0x20000, d0\n"
    "mov d0, a0\n"
    "mov a0, sp\n"
    "movm [a3], (sp)\n"
    "mov sp, a3\n"
    "add -12, sp\n"
    "mov 0, d1\n"
    "mov 0, d0\n"
    "call _main,[],0\n"
    "nop\n"
    "mov a3, sp\n"
    "break\n" // added cuz otherwise we just keep returning forever
    "ret [a3], 4\n");

// Need to make our own version of transparent_crc for the function
unsigned int returned = 0;
int print_hash_value = 0;
void transparent_crc(uint32_t val, char *vname, int flag) { returned ^= val; }
void crc32_gentab(void) {
  returned = 0;
  return;
}

// From https://github.com/gcc-mirror/gcc/blob/master/libiberty/memset.c
inline void *memset(void *dest, int val, size_t len) {
  unsigned char *ptr = (unsigned char *)dest;
  while (len-- > 0)
    *ptr++ = val;
  return dest;
}
// extern int main();
//  Need to provide our own entrypoint and hope it defaults to being at 0x0
//  otherwise will be a bit annoying innit.
// void _start(void) { main(); }
