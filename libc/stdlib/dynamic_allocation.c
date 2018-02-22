#include <stdlib.h>
#include <stddef.h>
#include <stdbool.h>
#include <string.h>

struct _metadata {
    bool used;
    size_t size;
} __attribute__((packed));

typedef struct _metadata metadata_t;

metadata_t *start;

void *malloc(size_t size) {
    if (size == 0)
        return NULL;

    //start = 0x0C;

    if (!start) {
        //give a page frame of space and init start
        start->used = true;
        start->size = size;
    }

    return NULL;
}

void *calloc(size_t n, size_t size) {
    void *ptr = malloc(n * size);
    if (ptr)
        memset(ptr, 0, size);
    return ptr;
}