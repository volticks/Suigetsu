#include "my_stdint.h"
#if 1
int x = 0;
#endif

#if 0
int x = 1;
#if 1
 X
#endif
#ifndef AAA
 X
#endif
#endif

int main(int argc, char **argv)
{
	return x;
}
