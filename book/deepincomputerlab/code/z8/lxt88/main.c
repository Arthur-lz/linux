#include "csapp.h"
#define SIGUSER1 	64 // 这个值最大是64，从65起程序将卡死，只能Ctrl+C退出 ; 
volatile unsigned long counter = 2;


void handler1(int sig)
{
/*	sigset_t mask, prev_mask;

	sigfillset(&mask);
	sigprocmask(SIG_BLOCK, &mask, &prev_mask);
//	sio_putl(--counter);
	sigprocmask(SIG_SETMASK, &prev_mask, NULL);
*/
	printf("%ld", --counter);
	fflush(stdout);
	_exit(0);
}

int main()
{
	pid_t pid;
//	sigset_t mask, prev_mask;

	printf("%ld", counter);
	fflush(stdout);

	signal(SIGUSER1, handler1);
	if ((pid = Fork()) == 0) {
		while(1){};
	}
	kill(pid, SIGUSER1);
	waitpid(-1, NULL, 0);

	//sigfillset(&mask);
	//sigprocmask(SIG_BLOCK, &mask, &prev_mask);
	printf("%ld", ++counter);
	//sigprocmask(SIG_SETMASK, &prev_mask, NULL);

	exit(0);
}
