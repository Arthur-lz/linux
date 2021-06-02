#include <stdio.h>
#include "rio.h"                                                                                                                                 
#include "open_clientfd_listenfd.h"
#include <sys/socket.h>
#include <sys/select.h>
#include <unistd.h>
#include "csapp.h"

typedef struct sockaddr SA;

void echo(int connfd)
{
	size_t n;
	char buf[MAXLINE];
	rio_t rio;

	rio_readinitb(&rio, connfd);
	while((n = rio_readlineb(&rio, buf, MAXLINE)) > 0) {
		//printf("server received %d bytes, msg: %s, receive: %s\n", (int)n, rio.rio_buf, buf);
		printf("server received %d bytes, receive: %s\n", (int)n, buf);
		//sprintf(buf, "server say: %s", buf);
		//rio_writen(connfd, buf, strlen(buf)); // server answer
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

int main(int argc, char **argv)
{
	int listenfd, connfd;
	socklen_t clientlen;
	struct sockaddr_storage clientaddr;
	char client_hostname[MAXLINE], client_port[MAXLINE];
	fd_set read_set, ready_set;

	if (argc != 2) {
		fprintf(stderr, "usage: %s <port>\n", argv[0]);
		exit(0);
	}

	listenfd = open_listenfd(argv[1]);
	FD_ZERO(&read_set);
	FD_SET(STDIN_FILENO, &read_set);
	FD_SET(listenfd, &read_set);

	while(1) {
		ready_set = read_set;
		select(listenfd+1, &ready_set, NULL, NULL, NULL);
		if (FD_ISSET(STDIN_FILENO, &ready_set)) {// STDIN_FILENO需引用unistd.h
			printf("stdin..\n");
			command();
		}
		if (FD_ISSET(listenfd, &ready_set)) {
			clientlen = sizeof(struct sockaddr_storage);
			connfd = accept(listenfd, (SA*)&clientaddr, &clientlen);
			getnameinfo((SA*)&clientaddr, clientlen, client_hostname, MAXLINE, client_port, MAXLINE, 0);
			printf("Connected to (%s, %s)\n", client_hostname, client_port);
			echo(connfd); // echo函数会一直在，除非客户端输入的消息中包含有over字符串才会结束echo
			close(connfd);// 关闭连接，之后本次建立的连接小命儿结束，这叫迭代echo服务器哈，连接一旦关闭，则客户端再发什么都无效
		}
	}
	exit(0);
}


