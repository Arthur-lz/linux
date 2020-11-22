#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <asm/fcntl.h>
#include <sys/mman.h>
//#include <sys/ioctl.h>
//#include <linux/fb.h>
//#include <string.h>

void main(void){
	int fd, i, len;
	char *addr;
	int offset = 4096;

	fd = open("a.txt", O_RDWR);
	if(fd < 0)
		exit(EXIT_FAILURE);

	len = 4096*2;
	addr = mmap(NULL, len, PROT_READ | PROT_WRITE, MAP_SHARED, fd, offset);
	if(addr == MAP_FAILED)
		exit(EXIT_FAILURE);


	for(i = 0;i< len; i++)
		printf("%c",*(addr+i));

}
