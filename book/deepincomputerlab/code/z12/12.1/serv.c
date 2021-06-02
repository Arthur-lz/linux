#include <stdio.h>
#include "rio.h"                                                                                                                                 
#include "open_clientfd_listenfd.h"
#include <sys/socket.h>
#include <sys/select.h>
#include <unistd.h>
#include "csapp.h"
#include <signal.h>

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

void sigchld_handler(int sig)
{
	while (waitpid(-1, 0, WNOHANG) > 0)
		;
	return;
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

	signal(SIGCHLD, sigchld_handler);

	listenfd = open_listenfd(argv[1]);

	while(1) {
		clientlen = sizeof(struct sockaddr_storage);
		connfd = accept(listenfd, (SA*)&clientaddr, &clientlen); // 服务器总是使用同一个监听描述符来监听不同的客户端，所以每一个返回的connfd都是对应不同的客户端的连接描述符，这一点一定要注意
		if (Fork() == 0) { // 这里是采用子进程来处理客户端请求，所以这个while循环体不会像别的那样阻塞在下面的echo函数中，它可以继续响应其他客户端的连接请求, 这是有别于１１章的客户端－服务器的迭代服务器模型的(11_4_8实验)
			getnameinfo((SA*)&clientaddr, clientlen, client_hostname, MAXLINE, client_port, MAXLINE, 0);
			printf("Connected to (%s, %s)\n", client_hostname, client_port);
			close(listenfd);
			echo(connfd); // echo函数会一直在，除非客户端输入的消息中包含有over字符串才会结束echo
			close(connfd);// 关闭连接，之后本次建立的连接小命儿结束，这叫迭代echo服务器哈，连接一旦关闭，则客户端再发什么都无效
			exit(0);
		}
		close(connfd);
	}
	exit(0);
}


