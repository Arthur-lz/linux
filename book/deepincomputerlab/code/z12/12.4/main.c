#include <pthread.h>
#include <stdio.h>

#define N 2

char **ptr;

void *threadfn(void *argv)
{
	int myid = (int)argv;
	static int cn = 0;
	printf("myid is %d, msg: %s, cn=%d\n", myid, ptr[myid], ++cn);
	return NULL;
}

int main(void)
{
	int i;
	pthread_t tid;
	char *msg[N] = {"hello foo",
		       "hello bar"
			};
	ptr = &msg[0];
	for (i = 0; i < N; i++) {
		pthread_create(&tid, NULL, threadfn, (void *)i);
	}
	//exit(0); exit(0)会直接退出主线程，而不会等待两个对等线程执行完, 它是用来终止进程其进程内所有线程的
	pthread_exit(NULL);
}
