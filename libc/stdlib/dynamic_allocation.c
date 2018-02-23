#include <stdlib.h>
#include <stddef.h>
#include <stdbool.h>
#include <string.h>
#include <kernel/heap.h>
#include <kernel/page.h>
#include <stdio.h>

#define NEXT_BLOCK(block) ((metadata_t*) (((uint32_t) ((block) + 1)) + (block)->size + sizeof(size_t)))
#define SIZE_PREV_BLOCK(block) (*(((size_t*) (block)) - 1))
#define PREV_BLOCK(block) (((metadata_t*) (((uint32_t) (block)) - sizeof(size_t) - SIZE_PREV_BLOCK((block)))) - 1)
#define ENDING_SIZE_FIELD(block) ((size_t*) (((uint32_t) ((block) + 1)) + (block)->size))
#define END_BLOCK(start, size) (((uint32_t) ((start) + 1)) + (size) + sizeof(size_t))

struct _metadata {
    bool used;
    size_t size;
} __attribute__((packed));

typedef struct _metadata metadata_t;

static metadata_t *start = NULL;
static metadata_t *end = NULL;
static uint32_t heap_end = 0;

void *malloc(size_t size) {
    if (size == 0)
        return NULL;

    if (!start || (uint32_t) start >= heap_end) {
        start = end = alloc_page(NUM_PAGES((uint32_t) start, END_BLOCK(start, size)));
        heap_end = ((uint32_t) start) + PAGE_SIZE;
        start->used = true;
        start->size = size;
        *ENDING_SIZE_FIELD(start) = size;
        return start + 1;
    }

    metadata_t *block = start;
    while (block <= end) {
        if (!block->used && block->size >= size) {
            block->used = true;
            //splits if there is room for 4 bytes
            if (block->size - size > sizeof(metadata_t) + sizeof(size_t) + 4) {
                size_t old_size = block->size;
                block->size = size;
                *ENDING_SIZE_FIELD(block) = size;
                metadata_t *next_block = NEXT_BLOCK(block);
                next_block->used = false;
                next_block->size = old_size - size - sizeof(metadata_t) - sizeof(size_t);
                *ENDING_SIZE_FIELD(next_block) = next_block->size;
            }   
            return block + 1;
        }
        block = NEXT_BLOCK(block);
    }
    end = NEXT_BLOCK(end);
    if (END_BLOCK(end, size) > heap_end) {
        size_t num_pages = NUM_PAGES((uint32_t) end, END_BLOCK(end, size));
        alloc_page(num_pages);
        heap_end += num_pages * PAGE_SIZE;
    }

    end->used = true;
    end->size = size;
    *ENDING_SIZE_FIELD(end) = size;
    return end + 1;
}

void *calloc(size_t n, size_t size) {
    void *ptr = malloc(n * size);
    if (ptr)
        memset(ptr, 0, size);
    return ptr;
}

void *realloc(void *ptr, size_t size) {
    void *new_ptr = malloc(size);
    memcpy(new_ptr, ptr, size);
    free(ptr);
    return new_ptr;
}

void free(void *ptr) {
    if (ptr) {
        metadata_t *block = ((metadata_t *) ptr) - 1;
        block->used = false;
        
        /* Could give back memory to but it makes the allocator much slower

        if (block == end && PAGE_BOUNDARY_END(block) < heap_end) {
            if (start != end)
                end = PREV_BLOCK(end);
            free_page((void*) PAGE_BOUNDARY_END(block), (heap_end - PAGE_BOUNDARY_END(block)) / PAGE_SIZE);
            heap_end = PAGE_BOUNDARY_END(block);
            return;
        }
        */

        // merge if possible
        if (block != end && !NEXT_BLOCK(block)->used) {
            if (NEXT_BLOCK(block) == end) {
                end = block;
            }
            block->size += sizeof(size_t) + sizeof(metadata_t) + NEXT_BLOCK(block)->size;
            *ENDING_SIZE_FIELD(block) = block->size;
        }

        if (block != start && !PREV_BLOCK(block)->used) {
            if (block == end) {
                end = PREV_BLOCK(block);
            }
            PREV_BLOCK(block)->size += sizeof(size_t) + sizeof(metadata_t) + block->size;
            *ENDING_SIZE_FIELD(block) = PREV_BLOCK(block)->size;
        }
    }
}