#include <pthread.h>
#include <stdio.h>
#include <sys/socket.h>
#include "sbuf.h"
#include "open_clientfd_listenfd.h"
#define SBUFSIZE	16
#define NTHREADS	4
typedef struct sockaddr SA;

volatile int byte_cn = 0;
sbuf_t sbuf;
void echo_cnt(int connfd);

void *threadfn(void *argv)
{
	int connfd;
	pthread_t id = pthread_self();
	pthread_detach(id);
	printf("tid %d in.\n", (int)id);	

	while ((connfd = sbuf_remove(&sbuf)) <= 0);

	echo_cnt(connfd);
	close(connfd);
	printf("tid %d over.\n", (int)id);	
	return NULL;
}

int main(int argc, char **argv)
{
	pthread_t tid;

	int i, listenfd, connfd;
	socklen_t 	clientlen;
	struct sockaddr_storage	clientaddr;

	if (argc != 2) {
		printf("usage: %s <port>\n", argv[0]);
		exit(0);
	}

	listenfd = open_listenfd(argv[1]);
	sbuf_init(&sbuf, SBUFSIZE);

	for (i = 0; i < NTHREADS; i++)
		pthread_create(&tid, NULL, threadfn, NULL);

	while (1) {
		clientlen = sizeof(struct sockaddr_storage);
		connfd = accept(listenfd, (SA*)&clientaddr, &clientlen);
		sbuf_insert(&sbuf, connfd);
	}
	exit(0);
}
