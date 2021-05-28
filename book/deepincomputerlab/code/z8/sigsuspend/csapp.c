#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdio.h>
#include <errno.h>

/* ltoa
 * 把value的值转换为以NULL结束的字符串，并把结果存在buf中。
 * radix是转换的基数值，在2到36之间。
 * 分配给buf的空间必须可容纳返回的所有字节（最多33字节）。
 * 
 * */
void itoa(unsigned long val,char *buf,unsigned radix)
{
	char *p;/*pointer to traverse string*/
	char *firstdig;/*pointer to first digit*/
	char temp;/*temp char*/
	unsigned digval;/*value of digit*/
 
	p = buf;
	firstdig = p;/*save pointer to first digit*/
 
	do{
		digval =(unsigned)(val % radix);
		val /= radix;/*get next digit*/
 
		/*convert to ascii and store */
		if(digval > 9)
			*p++ = (char)(digval - 10 + 'a ');/*a letter*/
		else 
			*p++ = (char)(digval + '0 ');/*a digit*/
	}while(val > 0);
 
		/*We now have the digit of the number in the buffer,but in reverse
		order.Thus we reverse them now.*/
 
	*p-- = '\0 ';/*terminate string;p points to last digit*/
 
	do{
		temp = *p;
		*p = *firstdig;
		*firstdig = temp;/*swap *p and *firstdig*/
		--p;
		++firstdig;/*advance to next two digits*/
	}while(firstdig < p);/*repeat until halfway*/
}

void unix_error(char *msg)
{
	fprintf(stderr, "%s: %s\n", msg, strerror(errno));
	exit(0);
}

pid_t Fork(void)
{
	pid_t pid;
	if ((pid = fork()) < 0)
		unix_error("Fork error");

	return pid;
}

ssize_t sio_puts(char s[])
{
	return write(STDOUT_FILENO, s, strlen(s));
}

ssize_t sio_putl(unsigned long v)
{
	char s[128];

	itoa(v, s, 10);
	return sio_puts(s);
}

void sio_error(char s[])
{
	sio_puts(s);
	_exit(1);
}

