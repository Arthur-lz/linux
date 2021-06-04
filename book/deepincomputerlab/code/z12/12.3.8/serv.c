#include <stdio.h>
#include "rio.h"                                                                                                                                 
#include "open_clientfd_listenfd.h"
#include <sys/socket.h>
#include <sys/select.h>
#include <unistd.h>
#include "csapp.h"
#include <signal.h>
#include <pthread.h>

typedef struct sockaddr SA;

void echo(int connfd)
{
	size_t n;
	char buf[MAXLINE];
	rio_t rio;

	rio_readinitb(&rio, connfd);
	while((n = rio_readlineb(&rio, buf, MAXLINE)) > 0) {
		printf("server received %d bytes, receive: %s\n", (int)n, buf);
		rio_writen(connfd, buf, n); // server answer, 这里如果修改n成其他值，客户端会出现没有收回服务器的回复，直到ctrl+c退出服务器端后，客户端才收到
		if (strstr(buf, "over")) // 当客户端输入的消息中包含over字符时，不再回显
			break;
	}
}

void command(void)
{
	char buf[MAXLINE];
	if (!fgets(buf, MAXLINE, stdin))
		exit(0);
	printf("%s", buf);
}

void *threadfn(void *argv)
{
	int connfd = *((int*)argv);
	pthread_detach(pthread_self());
	free(argv);
	echo(connfd);
	close(connfd);

	return NULL;
}

int main(int argc, char **argv)
{
	int listenfd, *connfd;
	socklen_t clientlen;
	struct sockaddr_storage clientaddr;
	char client_hostname[MAXLINE], client_port[MAXLINE];
	pthread_t tid;

	if (argc != 2) {
		fprintf(stderr, "usage: %s <port>\n", argv[0]);
		exit(0);
	}


	listenfd = open_listenfd(argv[1]);

	while(1) {
		clientlen = sizeof(struct sockaddr_storage);
		connfd = malloc(sizeof(int));
		*connfd = accept(listenfd, (SA*)&clientaddr, &clientlen); // 服务器总是使用同一个监听描述符来监听不同的客户端，所以每一个返回的connfd都是对应不同的客户端的连接描述符，这一点一定要注意
		
		pthread_create(&tid, NULL, threadfn, connfd);
	}
	exit(0);
}


