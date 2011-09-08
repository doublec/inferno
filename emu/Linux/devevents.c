#include "dat.h"
#include <linux/input.h>
#include <stdio.h>

void
event_process(struct input_event *ev, int count, int event_num) {
	int i;
	for(i = 0; i < count; i++) {
		char buf[80];
		int len;
		len = snprintf(buf, sizeof(buf), "%d %d %hu %d\n", event_num,  
			 ev[i].type, ev[i].code, ev[i].value);
		buf[len] = '\0';
		event_queue_produce(buf);
	}
}

void
eventslave(void *event_num) {
	int eventfd = eventfds[*((int *)event_num)];
	int rd, value, size = sizeof(struct input_event);
	struct input_event ev[64];
	int i;
	while (1){
		if ((rd = read (eventfd, ev, sizeof(ev))) < size) {
			print("read %d instead of %d\n", rd, size);
			sleep(1);
		}
		
		for (i = 0; i < rd / size; i++) {
			//print("ev[%d]: type = 0x%x, code = 0x%x, value = 0x%x\n", i, ev[i].type, ev[i].code, ev[i].value);
		}
		event_process(ev, rd / size, *((int *)event_num));
	}
	free(event_num);
}


