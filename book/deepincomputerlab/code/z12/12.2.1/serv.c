#include <stdio.h>
#include "rio.h"                                                                                                                                 
#include "open_clientfd_listenfd.h"
#include <sys/socket.h>
#include <sys/select.h>
#include <unistd.h>
#include "csapp.h"
#include <signal.h>

typedef struct sockaddr SA;

typedef struct {
	int maxfd;
	fd_set read_set;
	fd_set ready_set;
	int nready;
	int maxi;
	int clientfd[FD_SETSIZE];
	rio_t clientrio[FD_SETSIZE];
} pool;

int byte_cnt = 0;

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

void init_pool(int fd, pool *p)
{
	int i;
	p->maxi = -1;
	for (i = 0;i < FD_SETSIZE; i++)
		p->clientfd[i] = -1;

	p->maxfd = fd;
	FD_ZERO(&p->read_set);
	FD_SET(fd, &p->read_set);
}

void add_client(int connfd, pool *p)
{
	int i;

	for (i = 0; i < FD_SETSIZE; i++) {
		if (p->clientfd[i] < 0) {
			p->clientfd[i] = connfd;
			rio_readinitb(&p->clientrio[i], connfd);

			FD_SET(connfd, &p->read_set);

			if (connfd > p->maxfd)
				p->maxfd = connfd;
			if (i > p->maxi)
				p->maxi = i;
			break;
		}
	}

	if (i == FD_SETSIZE)
		unix_error("add_client error: Too many clients");
}

void check_clients(pool *p)
{
	int i, connfd, n;
	char buf[MAXLINE];

	rio_t rio;

	for (i = 0; (i <= p->maxi) && (p->nready > 0); i++) {
		connfd = p->clientfd[i];
		rio = p->clientrio[i];

		if ((connfd > 0) && (FD_ISSET(connfd, &p->ready_set))) {
			p->nready--;
			if ((n = rio_readlineb(&rio, buf, MAXLINE)) != 0) {
				byte_cnt += n;
				printf("Server received %d (%d total) bytes on fd %d\n", n, byte_cnt, connfd);
				rio_writen(connfd, buf, n);
			}
			else {
				close(connfd);
				FD_CLR(connfd, &p->read_set);
				p->clientfd[i] = -1;
			}
		}
	}
}

int main(int argc, char **argv)
{
	int listenfd, connfd;
	socklen_t clientlen;
	struct sockaddr_storage clientaddr;
	char client_hostname[MAXLINE], client_port[MAXLINE];
	static pool pool;

	if (argc != 2) {
		fprintf(stderr, "usage: %s <port>\n", argv[0]);
		exit(0);
	}

	listenfd = open_listenfd(argv[1]);
	init_pool(listenfd, &pool);

	while(1) {
		pool.ready_set = pool.read_set;
		pool.nready = select(pool.maxfd+1, &pool.ready_set, NULL, NULL, NULL);

		if (FD_ISSET(listenfd, &pool.ready_set)) {
			clientlen = sizeof(struct sockaddr_storage);
			connfd = accept(listenfd, (SA*)&clientaddr, &clientlen); // 服务器总是使用同一个监听描述符来监听不同的客户端，所以每一个返回的connfd都是对应不同的客户端的连接描述符，这一点一定要注意
			add_client(connfd, &pool);
		}
		check_clients(&pool);
	}
	exit(0);
}


