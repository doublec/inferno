#ifndef __AUDIT_H
#define __AUDIT_H
#define AUDITCHECK auditcheck(__FILE__, __LINE__)

void auditcheck(void);
void *auditmalloc(size_t orig);
void auditfree(void *p);
char *auditstrdup(char *str);
void *auditcalloc(size_t nmemb, size_t size);
void *auditrealloc(void *p, size_t size);
#endif
