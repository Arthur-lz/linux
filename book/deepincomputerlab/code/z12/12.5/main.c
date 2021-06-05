#include <pthread.h>
#include <stdio.h>

volatile int cn = 0;

void *threadfn(void *argv)
{
	int n = *((int*)argv);
	int i;

	for (i = 0; i < n; i++)
		cn++;
	return NULL;
}

int main(int argc, char **argv)
{
	pthread_t tid1, tid2;
	int i;

	if (argc != 2) {
		printf("usage: %s <niters>\n", argv[0]);
		exit(0);
	}

	i = atoi(argv[1]);

	pthread_create(&tid1, NULL, threadfn, &i);
	pthread_create(&tid2, NULL, threadfn, &i);
	pthread_join(tid1, NULL);
	pthread_join(tid2, NULL);

	if (cn != 2*i)
		printf("BOOM! cn is %d\n", cn);
	else
		printf("OK! cn is %d \n", cn);
	exit(0);
}
