#include <kernel/heap.h>
#include <stddef.h>
#include <kernel/page.h>
#include <stdio.h>

static uint32_t heap_end = 0;

void *alloc_page(size_t pages) {
    if (heap_end == 0) {
        heap_end = HEAP_START;
    }
    void *start_new_block = (void*) heap_end;
    for (size_t i = 0; i < pages; i++) {
        map_virtual_address(heap_end, 0x03);
        heap_end += PAGE_SIZE;
    }
    //printf("Heap End [Alloc]: %#.8X, Pages: %d\n", heap_end, pages);
    return start_new_block;
}

void free_page(void *start, size_t pages) {
    uint8_t *page = start;
    for (size_t i = 0; i < pages; i++) {
        unmap_virtual_address(page);
        page += PAGE_SIZE;
    }
    heap_end -= pages * PAGE_SIZE;
    //printf("Heap End [Free]: %#.8X, Pages: %d\n", heap_end, pages);
}