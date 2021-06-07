#include <stdio.h>

int main()
{
	union w {
		int a;
		char b;
	} c;

	c.a = 0x12345678;
	if (c.b == 0x78)
		printf("small, 0x%x\n", c.a);
	else if (c.b == 0x12)
		printf("Big\n");
	return 0;
}
