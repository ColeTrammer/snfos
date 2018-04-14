#ifndef _STDLIB_H
#define _STDLIB_H 1

#include <sys/cdefs.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

__attribute__((__noreturn__))
void abort(void);

#if defined(__is_kernel)
void *malloc_pages(size_t pages);
void  print_heap();
#endif

void *malloc(size_t size);
void *calloc(size_t items, size_t size);
void *realloc(void *ptr, size_t size);
void  free(void *ptr);

int atexit(void (*function)(void));
int atoi(const char *nptr);
char *getenv(const char *name);

__attribute__((__noreturn__))
void exit(int status);

#ifdef __cplusplus
}
#endif

#endif
