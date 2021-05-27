#include <stdio.h>
#include <wait.h>

int main(int argc, int *argv[])
{
	//int s = *argv[1];
	int s = 5;
	printf("%d, %d, %s\n", argc, s, argv[0]);
	int a = sleep(s);
	printf("Slept for %d of %d secs.\n", a, s);
	return 0;
}
