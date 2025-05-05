#include "my_stdint.h"
int (*fptr)() = 0;


int
main()
{
	if (fptr)
		return 1;
	return 0;
}

