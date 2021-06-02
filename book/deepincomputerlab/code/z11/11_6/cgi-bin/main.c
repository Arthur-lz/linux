#include <stdio.h>
#include <stdlib.h>

int main(int argc, char **argv)
{
	int ret = 0;
	char a, b;
	if (argc != 3) {
		printf("0\r\n");
		exit(0);
	}

	a = atoi(argv[1]);
	b = atoi(argv[2]);
	ret = a + b;
	printf("%d\r\n", ret);
	return ret;
}
