#define __is_libk 1

#include <kernel/module.h>
#include <kernel/page.h>
#include <kernel/heap.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <grub/multiboot.h>

process_t *load_module(multiboot_module_t *module) {
    process_t *process = malloc_pages(sizeof(process_t) / PAGE_SIZE);
	process->page_directory.entries[LAST_PAGE_DIRECTORY_INDEX] = get_physical_address(&process->page_directory) | 0x03;
	process->page_directory.entries[KERNEL_VIRTUAL_BASE >> 22] = KERNEL_PAGE_TABLE_PHYSICAL_ADDRESS | 0x03;
	process->page_directory.entries[0] = get_physical_address(&process->code_page_table) | 0x05;
	process->page_directory.entries[(KERNEL_VIRTUAL_BASE >> 22) - 1] = get_physical_address(&process->stack_page_table) | 0x07;
	
	process->stack_page_table.entries[LAST_PAGE_TABLE_INDEX] = get_physical_address((void*) alloc_page(1)) | 0x07;

    size_t num_code_pages = NUM_PAGES(module->mod_start, module->mod_end);
	//printf("Number of Code Pages: %d\n", num_code_pages);
	uint8_t *temp_pages = malloc_pages(num_code_pages);
	uint32_t phys_addr_temp_pages = get_physical_address(temp_pages);
	//printf("Temp Pages Virt Addr: %#.8X\n", temp_pages);
	//printf("Temp Pages Phys Addr: %#.8X\n", phys_addr_temp_pages);
	for (size_t i = 0; i < num_code_pages; i++) {
		map_page(((uint32_t) temp_pages) + (PAGE_SIZE * i), (module->mod_start + (PAGE_SIZE * i)) & 0xFFFFF000, 0x03);
		//printf("Phys Addr Mapped [%d]: %#.8X\n", i, ((module->mod_start + (PAGE_SIZE * i)) & 0xFFFFF000) - 0xC0000000);
	}

	uint8_t *program_code = malloc_pages(num_code_pages);
	for (size_t i = 0; i < num_code_pages; i++) { 
		process->code_page_table.entries[i] = get_physical_address(&program_code[i * PAGE_SIZE]) | 0x05;
	}

	memcpy(program_code, temp_pages, module->mod_end - module->mod_start);

	for (size_t i = 0; i < num_code_pages; i++) {
		free_physical_page((phys_addr_temp_pages + (PAGE_SIZE * i)) & 0xFFFFF000);
	}
    free(temp_pages);

    return process;
}