#include <stdio.h>

void test(char ***a)
{
	static char b = 'b';
	printf("static b addr=%p\n", &b);
	printf("test: a=%p, &a=%p, *a=%p\n", a, &a, *a);
	*a = &b;
	printf("test: a=%p, &a=%p, *a=%p\n", a, &a, *a);
}

void main()
{
	char i = 1;
	short t = 3;
	char **c = malloc(1);
	int j = 2;
	*c = malloc(1);
	char *d = *c;
	int *x;
	printf("main: c=%p, *c=%p, &x=%p\n", c, *c, &x);
	test(c);

	printf("main after test c=%p, *c=%p\n", c,*c);
	free(*d);
	free(c);
}
