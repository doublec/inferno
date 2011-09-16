#include "dat.h"
#include "fns.h"
#include "listenerqueue.h"

void
add_listener(struct listener **listeners, Queue *q)
{
	struct listener *node = malloc(sizeof(struct listener));

	node->next = NULL;
	node->q = q;

	if(*listeners == NULL) {
		*listeners = node;
	} else {
		struct listener *cur;
		for(cur = *listeners; ; cur = cur->next) {
			if(cur->next == NULL) {
				cur->next = node;
				break;
			}
		}
	}
}

void
del_listener(struct listener **listeners, Queue *q)
{
	struct listener *cur, *prev;

	prev = NULL;
	for(cur = *listeners; cur != NULL; cur = cur->next) {
		if(cur->q == q) {
			if(prev != NULL) {
				prev->next = cur->next;
			} else {
				*listeners = cur->next;
			}
			free(cur);
			break;
		}
		prev = cur;
	}
}

// Send a message to all queues in the event queue list
void
listeners_produce(struct listener **listeners, char *buf, size_t len)
{
	struct listener *cur;
	for(cur = *listeners; cur != NULL; cur = cur->next) {
		qproduce(cur->q, buf, len);
	}
}
