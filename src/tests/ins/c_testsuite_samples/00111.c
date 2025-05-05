#include "my_stdint.h"
int
main()
{
	short s = 1;
	long l = 1;

	s -= l;
	return s;
}
