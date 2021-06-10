#include <stdio.h>

typedef struct {
	int t;
	int val;
	char c;
}a;

void main()
{
	a A;
	A.val = 0;
	A.c = '1';
	char * ptr = &A.c;
	const typeof( ((a *)0)->c ) *__mptr = (ptr);
	a *B = (a *)((char*)__mptr - 8);
	//a *B = (a *)(ptr - 8);
	printf("A:%p, B:%p, __mptr:%p, ptr:%p\n", &A, B, __mptr, ptr);
}
