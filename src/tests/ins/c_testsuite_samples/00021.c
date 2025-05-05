#include "my_stdint.h"
int
foo(int a, int b)
{
	return 2 + a - b;
}

int
main()
{
	return foo(1, 3);
}

