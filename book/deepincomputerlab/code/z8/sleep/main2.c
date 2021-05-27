#include <stdio.h>
#include <wait.h>
#include "csapp.h"

void sigint_handler(int sig)
{
	return; // 默认这里会终止进程，现在这里在函数结尾未终止进程，那么从信号处理函数返回后可以继续执行main函数中sleep函数后面的指令
		 // 如果是原来的默认处理，则退出时会终止当前进程，那么就不会再执行main函数中sleep及其后面的指令
}

int main(int argc, char *argv[])
{
	int a;
	int s = 5;
	if (argc != 2)
		unix_error("param count error");

	s = atoi(argv[1]);
	if (signal(SIGINT, sigint_handler) == SIG_ERR)
		unix_error("signal error");

	printf("%d, %d, %s\n", argc, s, argv[0]);
	a = sleep(s);
	printf("Ctrl+C\n");
	printf("Slept for %d of %d secs.\n", a, s);
	exit(0);
}
