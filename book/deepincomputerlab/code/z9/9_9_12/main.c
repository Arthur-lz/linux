#include <stdio.h>

void main(void)
{
	int a[5] = {0, 1, 2, 3, 4};
	int *b = &a[0];
	int *c = b;

	b++;
	printf("b is %p, *b is %d\n", b, *b);
	printf("c is %p, *c is %d\n", c, *c);

}
