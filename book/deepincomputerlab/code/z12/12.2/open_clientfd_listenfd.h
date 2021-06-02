#define MAXLINE 255
#define LISTENQ 1024

int open_clientfd(char *hostname, char *port);
int open_listenfd(char *port);
