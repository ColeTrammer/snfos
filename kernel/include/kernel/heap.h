#ifndef _KERNEL_HEAP_H
#define _KERNEL_HEAP_H

#include <stddef.h>
#include <kernel/page.h>

#define HEAP_START (PAGE_BOUNDARY_END(VIRT_KERNEL_END) + PAGE_SIZE)

void *alloc_page(size_t pages);
void  free_page(void *start, size_t pages);

#endif