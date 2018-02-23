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
#define MAX_PAGE_DIRECTORY_ENTRIES 1024
#define MAX_PAGE_TABLE_ENTRIES 1024
#define LAST_PAGE_DIRECTORY_INDEX (MAX_PAGE_DIRECTORY_ENTRIES - 1)
#define LAST_PAGE_TABLE_INDEX (MAX_PAGE_TABLE_ENTRIES - 1)
#define PAGE_DIRECTORY ((page_directory_t*) ((LAST_PAGE_DIRECTORY_INDEX << 22) | (LAST_PAGE_TABLE_INDEX << 12)))
#define PAGE_TABLE(offset) ((page_table_t*) ((LAST_PAGE_DIRECTORY_INDEX << 22) | ((offset) << 12)))
#define PAGE_BOUNDARY_START(addr) (((uint32_t) (addr)) & 0xFFFFF000)
#define PAGE_BOUNDARY_END(addr) (PAGE_BOUNDARY_START((addr)) + PAGE_SIZE)
#define NUM_PAGES(start, end) (((end) / PAGE_SIZE) - ((start) / PAGE_SIZE) + ((end) % PAGE_SIZE != 0 ? 1 : 0))

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

struct _page_directory {
    uint32_t entries[MAX_PAGE_DIRECTORY_ENTRIES];
} __attribute__((packed));

typedef struct _page_directory page_directory_t;

struct _page_table {
    uint32_t entries[MAX_PAGE_TABLE_ENTRIES];
} __attribute__((packed));

typedef struct _page_table page_table_t;

uint32_t get_physical_address(void *virtual_address);

void init_bitmap(multiboot_info_t *info);
uint32_t allocate_next_page();
void  free_physical_page(uint32_t page);
void *map_virtual_address(uint32_t virtual_address, uint16_t flags);
void *map_page(uint32_t virtual_address, uint32_t physical_address, uint16_t flags);
void  unmap_virtual_address(void *virtual_address);

#endif