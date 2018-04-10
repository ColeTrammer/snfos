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
#endif

void *malloc(size_t size);
void *calloc(size_t items, size_t size);
void *realloc(void *ptr, size_t size);
void  free(void *ptr);

#ifdef __cplusplus
}
#endif

#endif
