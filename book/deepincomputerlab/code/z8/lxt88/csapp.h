#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <signal.h>

void unix_error(char *msg);
pid_t Fork(void);
ssize_t sio_putl(long v);
ssize_t sio_puts(char s[]);

void sio_error(char s[]);
