#include <kernel/page.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <grub/multiboot.h>
#include <stdio.h>

static bool freed = false;
static uint32_t *last_allocated = NULL;

void *allocate_next_page() {
    for (uint32_t *block = !freed && last_allocated ? last_allocated : BITMAP_START; block < BITMAP_END; block++) {
        if (~(*block)) {
            uint32_t mask = 1;
            uint8_t bit = 0;
            while (1) { 
                if ((*block & mask) == 0) {
                    (*block) |= mask;
                    last_allocated = block;
                    freed = false;
                    return (void*) (((block - BITMAP_START) * 32 + bit) * PAGE_SIZE);
                }
                mask <<= 1;
                bit++;
            }
        }
    }        
    return NULL;
}

void free_page(void *page) {
    uint32_t addr = (uint32_t) page;
    addr &= 0xFFFFF000;
    uint32_t offset = addr / PAGE_SIZE / 32;
    uint8_t bit_num = (addr % 32);
    uint32_t update = ~(1 << bit_num);
    *(BITMAP_START + offset) &= update;
    freed = true;
}

void mark_used(void *start, size_t num) {
    uint32_t addr = (uint32_t) start;
    addr &= 0xFFFFF000;
    uint32_t offset = addr / PAGE_SIZE / 32;
    uint32_t bit_num = (addr / PAGE_SIZE) % 32;
    while (num > 0) {
        uint32_t update = 0;
        while (bit_num < 32 && num > 0) {
            update |= (1 << bit_num);
            bit_num++;
            num--;
        }
        *(BITMAP_START + offset) |= update;
        offset++;
        bit_num = 0;
    }
}

void init_bitmap(multiboot_info_t *info) {
    mark_used((void*) 0, PHYS_KERNEL_START / PAGE_SIZE);//Reserves 1MB
    mark_used((void*) PHYS_KERNEL_START, NUM_INITIAL_KERNEL_PAGES);
    multiboot_module_t *modules = (multiboot_module_t*) info->mods_addr;
    for (size_t i = 0; i < info->mods_count; i++) {
        mark_used((void*) modules[i].mod_start - 0xC0000000, NUM_PAGES(modules[i].mod_start, modules[i].mod_end));
    }

    if (info->flags & (1 << 6)) {
        uint32_t *mmap_start = (uint32_t*) info->mmap_addr;
        uint32_t *mmap = mmap_start;
        uint32_t length = info->mmap_length;
        while ((uint32_t) mmap < ((uint32_t) mmap_start) + length) {
            mmap++;//size field
            uint32_t start = *mmap;//start field
            mmap += 2;
            uint32_t end = start + *mmap;//length field
            if (end == 0) 
                end = UINT32_MAX;
            mmap += 2;
            uint32_t type = *mmap;//type field
            mmap++;
            if (type != 1) {//reserved
                mark_used((void*) start, NUM_PAGES(start, end));
            }
        }
    }
}