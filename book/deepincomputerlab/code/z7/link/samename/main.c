#include <stdio.h>

int x = 5678;
int y = 1234;

void f();
void main()
{
	f();
	printf("xAddr is 0x%x, yAddr is 0x%x\n", &x, &y);
	printf("x=0x%x, y=0x%x\n", x,y);
}
