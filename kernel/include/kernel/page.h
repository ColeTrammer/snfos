#ifndef _KERNEL_PAGE_H
#define _KERNEL_PAGE_H

#include <stdint.h>
#include <grub/multiboot.h>

extern void page_bitmap(void);
extern void kernel_virtual_start(void);
extern void kernel_physical_start(void);
extern void kernel_virtual_end(void);
extern void kernel_physical_end(void);

#define PAGE_SIZE 4096
#define NUM_PAGES(start, end) ((end / PAGE_SIZE) - (start / PAGE_SIZE) + (end % PAGE_SIZE != 0 ? 1 : 0))

#define BITMAP_PAGES 32
#define BITMAP_SIZE (PAGE_SIZE * BITMAP_PAGES)
#define BITMAP_ADDRESS ((uint32_t) &page_bitmap)
#define BITMAP_START ((uint32_t*) BITMAP_ADDRESS)
#define BITMAP_END ((uint32_t*) (BITMAP_ADDRESS + BITMAP_SIZE))

#define VIRT_KERNEL_START ((uint32_t) &kernel_virtual_start)
#define PHYS_KERNEL_START ((uint32_t) &kernel_physical_start)
#define VIRT_KERNEL_END ((uint32_t) &kernel_virtual_end)
#define PHYS_KERNEL_END ((uint32_t) &kernel_physical_end)
#define KERNEL_SIZE (VIRT_KERNEL_END - VIRT_KERNEL_START)
#define NUM_INITIAL_KERNEL_PAGES (NUM_PAGES(PHYS_KERNEL_START, PHYS_KERNEL_END))

void init_bitmap(multiboot_info_t *info);
void *allocate_next_page();
void  free_page(void *page);

#endif