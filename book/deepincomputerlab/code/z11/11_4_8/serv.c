#include <stdio.h>
#include "rio.h"                                                                                                                                 
#include "open_clientfd_listenfd.h"
#include <sys/socket.h>
typedef struct sockaddr SA;

void echo(int connfd)
{
	size_t n;
	char buf[MAXLINE];
	rio_t rio;

	rio_readinitb(&rio, connfd);
	while((n = rio_readlineb(&rio, buf, MAXLINE)) != 0) {
		//printf("server received %d bytes, msg: %s, receive: %s\n", (int)n, rio.rio_buf, buf);
		printf("server received %d bytes, receive: %s\n", (int)n, buf);
		//sprintf(buf, "server say: %s", buf);
		//rio_writen(connfd, buf, strlen(buf)); // server answer
		rio_writen(connfd, buf, n); // server answer, 这里如果修改n成其他值，客户端会出现没有收回服务器的回复，直到ctrl+c退出服务器端后，客户端才收到
	}
}

int main(int argc, char **argv)
{
	int listenfd, connfd;
	socklen_t clientlen;
	struct sockaddr_storage clientaddr;
	char client_hostname[MAXLINE], client_port[MAXLINE];

	if (argc != 2) {
		fprintf(stderr, "usage: %s <port>\n", argv[0]);
		exit(0);
	}

	listenfd = open_listenfd(argv[1]);
	while(1) {
		clientlen = sizeof(struct sockaddr_storage);
		connfd = accept(listenfd, (SA*)&clientaddr, &clientlen);
		getnameinfo((SA*)&clientaddr, clientlen, client_hostname, MAXLINE, client_port, MAXLINE, 0);
		printf("Connected to (%s, %s)\n", client_hostname, client_port);
		echo(connfd);
		close(connfd);
	}
	exit(0);
}
