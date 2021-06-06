#include <semaphore.h>
#include <pthread.h>

#define MAXTHREADS	32
#define P	sem_wait
#define V	sem_post

void *sum_mutex(void *vargp);
void *sum_local(void *argp);

unsigned long gsum = 0;
long nelems_per_thread;
sem_t mutex;
unsigned long psum[];

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
		pthread_create(&tid[i], NULL, sum_local, &myid[i]);
	}

	for (i = 0; i < nthreads; i++) {
		pthread_join(tid[i], NULL);
	}

	for (i = 0; i < nthreads; i++)
		gsum += psum[i];
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
		P(&mutex);
		gsum += i;
		V(&mutex);
	}
	return NULL;
}

void *sum_local(void *vargp)
{
	long myid = *((int*)vargp);
	long start = myid * nelems_per_thread;
	long end = start + nelems_per_thread;

	long i;
	unsigned long sum = 0;

	for (i = start; i < end; i++) {
		sum += i;
	}
	psum[myid] = sum;
	return NULL;
	
}
