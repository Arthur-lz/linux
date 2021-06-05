#include "sbuf.h"

void sbuf_init(sbuf_t *sp, int n)
{
	sp->buf = calloc(n, sizeof(int));
	sp->n = n;
	sp->front = sp->rear = 0;
	sem_init(&sp->mutex, 0, 1);
	sem_init(&sp->slots, 0, 1);
	sem_init(&sp->items, 0, 1);
}

void sbuf_deinit(sbuf_t *sp)
{
	free(sp->buf);
}

void sbuf_insert(sbuf_t *sp, int item)
{
	P(&sp->slots);
	P(&sp->mutex);
	sp->buf[(++sp->rear)%(sp->n)] = item;
	V(&sp->mutex);
	V(&sp->slots);
}

int sbuf_remove(sbuf_t *sp)
{
	int item = -1;
	P(&sp->items);
	P(&sp->mutex);
	item = sp->buf[(++sp->front)%(sp->n)];
	V(&sp->mutex);
	V(&sp->items);

	return item;
}
