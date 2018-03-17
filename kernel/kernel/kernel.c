#define __is_libk 1

#include <stdio.h>
#include <kernel/tty.h>
#include <kernel/input.h>
#include <grub/multiboot.h>
#include <stdbool.h>
#include <kernel/page.h>
#include <kernel/heap.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

extern void load_program(uint32_t phys_addr);

unsigned int to_virtual(unsigned int in) {
	return in + 0xC0000000;
}

multiboot_info_t *virtualize_multiboot_info(unsigned int ebx) {
	multiboot_info_t *mbinfo = (multiboot_info_t*) to_virtual(ebx);
	mbinfo->mods_addr = to_virtual(mbinfo->mods_addr);
	mbinfo->mmap_addr = to_virtual(mbinfo->mmap_addr);

	multiboot_module_t *module = (multiboot_module_t*) mbinfo->mods_addr;
	module->mod_start = to_virtual(module->mod_start);
	module->mod_end = to_virtual(module->mod_end);
	module->cmdline = to_virtual(module->cmdline);
	return mbinfo;	
}

void kernel_main(uint32_t eax, uint32_t ebx) {
	
	terminal_initialize();
	if (eax != 0x2BADB002) {
		printf("ERROR: INVALID BOOTLOADER\neax: %#.8X\nebx: %#.8X\n", eax, ebx);
		return;
	}

	multiboot_info_t *mbinfo = virtualize_multiboot_info(ebx);
	init_bitmap(mbinfo);
	
	initialize_input();
	
	typedef struct _process {
		page_directory_t page_directory;
		page_table_t code_page_table;
		page_table_t stack_page_table;
	} __attribute__((packed)) process_t;

	process_t *process = malloc_pages(sizeof(process_t) / PAGE_SIZE);
	process->page_directory.entries[LAST_PAGE_DIRECTORY_INDEX] = get_physical_address(&process->page_directory) | 0x03;
	process->page_directory.entries[KERNEL_VIRTUAL_BASE >> 22] = KERNEL_PAGE_TABLE_PHYSICAL_ADDRESS | 0x03;
	process->page_directory.entries[0] = get_physical_address(&process->code_page_table) | 0x05;
	process->page_directory.entries[(KERNEL_VIRTUAL_BASE >> 22) - 1] = get_physical_address(&process->stack_page_table) | 0x07;
	
	process->stack_page_table.entries[LAST_PAGE_TABLE_INDEX] = get_physical_address((void*) alloc_page(1)) | 0x07;
	/* TESTS */
	
	multiboot_module_t *module = (multiboot_module_t*) mbinfo->mods_addr;
	void *temp_page = map_page(HEAP_START - PAGE_SIZE, module->mod_start - 0xC0000000, 0x03);
	
	size_t num_code_pages = ((module->mod_end - module->mod_start) / PAGE_SIZE) + 1;
	uint8_t *program_code = malloc_pages(num_code_pages);
	for (size_t i = 0; i < num_code_pages; i++) { 
		process->code_page_table.entries[i] = get_physical_address(&program_code[i * PAGE_SIZE]) | 0x05;
	}

	memcpy(program_code, temp_page, module->mod_end - module->mod_start);

	unmap_virtual_address(temp_page);

	if (mbinfo->mods_count == 1 && mbinfo->flags & (1 << 3)) {
		printf("Start: %#.8X\nEnd:   %#.8X\nString: %s\n", module->mod_start, module->mod_end, module->cmdline);
		printf("Process:        %#.8X\n", process);
		printf("Page Directory: %#.8X\n", &process->page_directory);
		printf("Kernel PT:      %#.8X\n", KERNEL_PAGE_TABLE_PHYSICAL_ADDRESS);
		load_program(get_physical_address(&process->page_directory));	
	}		

/*
	free(malloc(8));
	for (size_t i = 1; i < 10000000; i++) {
		uint8_t *ptr = malloc(i);
		ptr[i - 1] = 0xAB;
		free(ptr);
	}
	printf("%s\n", "finished");
*/

/*
	uint8_t* a = malloc(8);
	uint8_t* b = malloc(8);
	uint8_t* c = malloc(8);
	uint8_t* d = malloc(8);

	printf("A: %#.8X  B: %#.8X C: %#.8X D: %#.8X E: %.8X\n", a, b, c, d, 0);
	free(b);
	uint8_t* e = malloc(7);
	printf("A: %#.8X  B: %#.8X C: %#.8X D: %#.8X E: %.8X\n", a, 0, c, d, e);
	free(c);
	free(d);
	b = malloc(16);
	printf("A: %#.8X  B: %#.8X C: %#.8X D: %#.8X E: %.8X\n", a, b, 0, 0, e);

	//free(b);
	//b = malloc(64);

	c = malloc(128);
	d = malloc(8);
	printf("A: %#.8X  B: %#.8X C: %#.8X D: %#.8X E: %.8X\n", a, b, c, d, e);
	free(c);
	free(d);
	c = malloc(8);
	d = malloc(8);
	printf("A: %#.8X  B: %#.8X C: %#.8X D: %#.8X E: %.8X\n", a, b, c, d, e);
*/
/*	
	for (size_t j = 0; j < 10; j++) {
		for (size_t i = 0; i < 5; i++) {
			printf("%#07X ", allocate_next_page());
		}
		printf("%s", "\n");
	}

	// printf("%*.*s____%-3c%3c\n", 6, 5, "ABCD", '+', '-');
	// printf("%+-#12X<>\n", 0x43A);
	// printf("%+0#*d\n", 8, 91235);
*/

	while (1);
}