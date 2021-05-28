#include "csapp.h"

volatile sig_atomic_t pid;

void sigchld_handler(int s)
{
	int olderrno = errno;
	pid = waitpid(-1, NULL, 0);
	errno = olderrno;
}

void sigint_handler(int s)
{

}

int main(int argc, char **argv)
{
	sigset_t mask, prev;

	signal(SIGCHLD, sigchld_handler);
	signal(SIGINT, sigint_handler);
	sigemptyset(&mask);
	sigaddset(&mask, SIGCHLD);

	while(1) {
		sigprocmask(SIG_BLOCK, &mask, &prev);
		if (Fork() == 0)
			exit(0);

		pid = 0;
		while(!pid) // 这个循环用来在父进程中等待子进程退出
			sigsuspend(&prev); 
		/* sigsuspend等价于下面三句
		 * sigprocmask(SIG_SETMASK, &prev, &mask); // 打开对SIGCHLD信号的接收
		 * pause();
		 * sigprocmask(SIG_SETMASK, &mask, NULL);  // 关闭对SIGCHLD信号的接收
		 * */

		sigprocmask(SIG_SETMASK, &prev, NULL);

		printf(".");
	}
	exit(0);
}
