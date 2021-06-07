#include <semaphore.h>
#include <pthread.h>

#define MAXTHREADS	32
#define P	sem_wait
#define V	sem_post

void *sum_mutex(void *vargp);

unsigned long gsum = 0;
long nelems_per_thread;
sem_t mutex;

int main(int argc, char **argv)
{
	unsigned long i, nelems, log_nelems, nthreads, myid[MAXTHREADS];
	pthread_t tid[MAXTHREADS];

	if (argc != 3) {
		printf("usage: %s <nthreads> <log_nelems>\n", argv[0]);
		exit(0);
	}

	nthreads = atoi(argv[1]);
	log_nelems = atoi(argv[2]);
	nelems = (1 << log_nelems);
	nelems_per_thread = nelems / nthreads;
	sem_init(&mutex, 0, 1);

	for (i = 0; i < nthreads; i++) {
		myid[i] = i;
		pthread_create(&tid[i], NULL, sum_mutex, &myid[i]);
	}

	for (i = 0; i < nthreads; i++) {
		pthread_join(tid[i], NULL);
	}

	if (gsum != (nelems * (nelems - 1))/2)
		printf("Error: result = %ld\n", gsum);
	else
		printf("OK, result = %ld\n", gsum);
	exit(0);
}


void *sum_mutex(void *vargp)
{
	long myid = *((int*)vargp);
	long start = myid * nelems_per_thread;
	long end = start + nelems_per_thread;

	long i;

	for (i = start; i < end; i++) {
		P(&mutex); // 如何去掉同步操作？首先明确这里同步操作是因为访问了全局变量gsum，所以最简单的方式是给每一个线程分一个独立的内存来存sum值，简单点可以使用数组，如有Ｎ个线程，则申请一个全局的Ｎ个元素的数组来分别保存每一个线程中需要操作的sum值，这样就用每个线程自己的数组成员来避免了与多个线程共享同一个gsum，从而可删除掉这里的ＰＶ操作
		gsum += i;
		V(&mutex);
	}
	return NULL;
}
