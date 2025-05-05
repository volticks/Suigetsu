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
//

#ifndef __x86_64__
inline void *memcpy(void *dest, const void *src, size_t n) {
  // asm("break\n");
  for (size_t i = 0; i < n; i++) {
    ((char *)dest)[i] = ((char *)src)[i];
  }
  return dest;
}

// extern int main(int argc, char **argv);

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
    "mov 0x9000, d0\n"
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

// extern int main(int argc, char **argv);
//  Need to provide our own entrypoint and hope it defaults to being at 0x0
//  otherwise will be a bit annoying innit.
// void _start(void) { main(); }
unsigned long strlen(const char *str) {
  int sz = 0;
  while (1) {
    if (str[sz] == 0)
      break;
    sz++;
  }
  return sz;
}
#endif

#ifdef __x86_64__

#include <stddef.h>
#include <stdint.h>
#include <string.h>

#endif
