#include <stdlib.h>
#include <stdio.h>

struct alloc_node {
	void *block;
	struct alloc_node *next;
};

struct alloc_node *alloc_head = NULL;

void add_alloc_node(void *block)
{
	struct alloc_node *node = malloc(sizeof(struct alloc_node));
	struct alloc_node *cur;

	node->block = block;
	node->next = NULL;
	if(alloc_head == NULL) {
		alloc_head = node;
	} else {
		for(cur = alloc_head; ; cur = cur->next) {
			if(cur->next == NULL) {
				cur->next = node;
				break;
			}
		}
	}
}

void del_alloc_node(void *block)
{
	struct alloc_node *cur;
	struct alloc_node *prev;

	for(cur = alloc_head; cur != NULL; cur = cur->next) {
		if(cur->block == block) {
			if(cur == alloc_head) {
				alloc_head = cur->next;
			} else {
				prev->next = cur->next;
			}
			free(cur);
			return;
		}
		prev = cur;
	}
	fprintf(stderr, "del_alloc_node: could not find block %p\n", block);
}

void auditcheck(char *file, int line)
{
	struct alloc_node *cur;
	for(cur = alloc_head; cur != NULL; cur = cur->next) {
		void *p = cur->block;
		long hdr, canary;
		size_t orig;
		
		hdr = *((long *)p - 2);
		if(hdr != 0xcafeface) {
			fprintf(stderr, "auditcheck (from %s:%d): bad header at block %p, found %x\n",
				file, line, p, canary);
		}
		orig = *((size_t *) p - 1);
		canary = *((long *)(p + orig));
		if(canary != 0xaaaaaaaa) {
			fprintf(stderr, "auditcheck (from %s:%d): bad canary at block %p, found %x\n",
				file, line, p, canary);
		}
	}
}

void *auditmalloc(size_t orig)
{
	size_t size = orig + sizeof(size_t) + 2*sizeof(long);
	void *ret;
	ret = malloc(size);
	if(ret == NULL) return NULL;
	*((long *) ret) = 0xcafeface;
	*(((size_t *) ret) + 1) = orig;
	*((long *) ((char *)ret + orig + sizeof(long) + sizeof(size_t))) = 0xaaaaaaaa;
	ret += sizeof(long) + sizeof(size_t);
	add_alloc_node(ret);
	return ret; 
}

void auditfree(void *p)
{
	long hdr, canary;
	size_t orig;

	if(p == NULL) return;
	
	hdr = *((long *)p - 2);
	if(hdr != 0xcafeface) {
		fprintf(stderr, "auditfree: bad header at block %p, found %x\n",
			p, canary);
	}
	orig = *((size_t *) p - 1);
	canary = *((long *)(p + orig));
	if(canary != 0xaaaaaaaa) {
		fprintf(stderr, "auditfree: bad canary at block %p, found %x\n",
			p, canary);
	}
	del_alloc_node(p);
	p = p - sizeof(long) - sizeof(size_t);
	memset(p, 0xbadbad, orig + 2 * sizeof(long) + sizeof(size_t));
	free(p);
}

char *auditstrdup(char *str)
{
	char *ret;
	ret = auditmalloc((strlen(str) + 1) * sizeof(char));
	strcpy(ret, str);
	return ret;
}

void *auditcalloc(size_t nmemb, size_t size)
{
	size_t total = nmemb * size;
	void *ret;
	ret = auditmalloc(total);
	memset(ret, 0, total);
	return ret;
}


void *auditrealloc(void *p, size_t size)
{
	void *ret;
	size_t orig = size;
	del_alloc_node(p);
	p = p - sizeof(size_t) - sizeof(long);
	size += 2 * sizeof(long) + sizeof(size_t);
	ret = realloc(p, size);
	*(((size_t *) ret) + 1) = orig;

	ret += sizeof(size_t) + sizeof(long);
	*((long *)(ret + orig)) = 0xaaaaaaaa;
	add_alloc_node(ret);
	return ret;
}
