#include "sbuf.h"

void sbuf_init(sbuf_t *sp, int n)
{
	sp->buf = calloc(n, sizeof(int));
	sp->n = n;
	sp->front = sp->rear = 0;
	sem_init(&sp->mutex, 0, 1); // 保护临界区
	sem_init(&sp->slots, 0, n); // n个slots
	sem_init(&sp->items, 0, 0); // 无可用的
}

void sbuf_deinit(sbuf_t *sp)
{
	free(sp->buf);
}

void sbuf_insert(sbuf_t *sp, int item)
{
	P(&sp->slots); // 有空巢吗？有的话，则将可以装数据的空巢减少一个
	P(&sp->mutex); // 进入临界区
	sp->buf[(++sp->rear)%(sp->n)] = item;
	V(&sp->mutex); // 退出临界区
	V(&sp->items); // 将可用项目加１, 即生产者生产新数据由sp->items的值来体现
}

int sbuf_remove(sbuf_t *sp)
{
	int item = -1;
	P(&sp->items); // 可用项目大于０？即，现在如果没有可用项，则会阻塞在这里
	P(&sp->mutex); // 进入临界区
	item = sp->buf[(++sp->front)%(sp->n)];
	V(&sp->mutex); // 退出临界区
	V(&sp->slots); // 消费者取走了一个可用项，所以需要将空巢加１

	return item;
}
