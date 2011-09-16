#ifndef __LISTENERQUEUE_H
#define __LISTENERQUEUE_H
struct listener {
	Queue *q;
	struct listener *next;
};

void add_listener(struct listener **listeners, Queue *q);
void del_listener(struct listener **listeners, Queue *q);
void listeners_produce(struct listener **listeners, char *buf, size_t len);
#endif
