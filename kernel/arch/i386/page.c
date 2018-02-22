#include <kernel/page.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <grub/multiboot.h>
#include <stdio.h>
#include <string.h>

static inline void invlpg(void* m)
{
    /* Clobber memory to avoid optimizer re-ordering access before invlpg, which may cause nasty bugs. */
    asm volatile ( "invlpg (%0)" : : "b"(m) : "memory" );
}

static bool freed = false;
static uint32_t *last_allocated = NULL;

uint32_t allocate_next_page() {
    for (uint32_t *block = !freed && last_allocated ? last_allocated : BITMAP_START; block < BITMAP_END; block++) {
        if (~(*block)) {
            uint32_t mask = 1;
            uint8_t bit = 0;
            while (1) { 
                if ((*block & mask) == 0) {
                    (*block) |= mask;
                    last_allocated = block;
                    freed = false;
                    return ((block - BITMAP_START) * 32 + bit) * PAGE_SIZE;
                }
                mask <<= 1;
                bit++;
            }
        }
    }        
    return 0;
}

void free_page(uint32_t page) {
    uint32_t addr = page;
    addr &= 0xFFFFF000;
    uint32_t offset = addr / PAGE_SIZE / 32;
    uint8_t bit_num = (addr / PAGE_SIZE) % 32;
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

void *map_virtual_address(uint32_t virtual_address, uint16_t flags) {
    return map_page(virtual_address, allocate_next_page(), flags);
}

void *map_page(uint32_t virtual_address, uint32_t physical_address, uint16_t flags) {
    virtual_address  &= 0xFFFFF000;    //guantees address is on the page boundary
	physical_address &= 0xFFFFF000;

	uint32_t page_directory_offset = virtual_address >> 22;
	uint32_t page_table_offset = (virtual_address >> 12) & 0x03FF;  //gets first 10 digits
	
    page_directory_t *page_directory = PAGE_DIRECTORY;
    page_table_t *page_table = PAGE_TABLE(page_directory_offset);
    if (!(page_directory->entries[page_directory_offset] & 1)) {    //missing page table
        uint32_t phys_page_addr = (uint32_t) allocate_next_page();
        page_directory->entries[page_directory_offset] = phys_page_addr | 0x03; //maps in page table
        memset(page_table, 0, PAGE_SIZE);
    }

	page_table->entries[page_table_offset] = physical_address | (flags & 0xFFF) | 1;
    return (void*) virtual_address;
}

void unmap_virtual_address(void *_virtual_address) {
    uint32_t virtual_address = ((uint32_t) _virtual_address) & 0xFFFFF000;

    uint32_t page_directory_offset = virtual_address >> 22;
    uint32_t page_table_offset = (virtual_address >> 12) & 0x03FF;

    page_table_t *page_table = PAGE_TABLE(page_directory_offset);
    
    uint32_t phys_addr = page_table->entries[page_table_offset]; // dont need to normalize address to page bound b/c free page does this automatically
    page_table->entries[page_table_offset] = 0;                  // clear page entry
    invlpg((void*) virtual_address);                                     // clear page cache
    free_page(phys_addr);                                        // marks page as usable

    for (size_t i = 0; i < MAX_PAGE_TABLE_ENTRIES; i++) {        // checks to see if any page table entry is presenet
        if (page_table->entries[i] & 1)
            return;
    }
    unmap_virtual_address(page_table);                           // removes PT if not
}

uint32_t get_physical_address(void *_virtual_address) {
    uint32_t virtual_address = ((uint32_t) _virtual_address) & 0xFFFFF000;

    uint32_t page_directory_offset = virtual_address >> 22;
    uint32_t page_table_offset = (virtual_address >> 12) & 0x03FF;

    page_table_t *page_table = PAGE_TABLE(page_directory_offset);
    return page_table->entries[page_table_offset] & 0xFFFFF000;
}

void init_bitmap(multiboot_info_t *info) {
    mark_used((void*) 0, PHYS_KERNEL_START / PAGE_SIZE);                //Reserves 1MB
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