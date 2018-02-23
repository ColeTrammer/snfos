#ifndef _KERNEL_HEAP_H
#define _KERNEL_HEAP_H

#include <stddef.h>

#define HEAP_START 0x100000

void *alloc_page(size_t pages);
void  free_page(void *start, size_t pages);

#endif