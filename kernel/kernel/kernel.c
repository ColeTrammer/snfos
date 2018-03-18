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
#include <kernel/module.h>

extern void load_program(uint32_t phys_addr);

unsigned int to_virtual(unsigned int in) {
	return in + 0xC0000000;
}

multiboot_info_t *virtualize_multiboot_info(unsigned int ebx) {
	multiboot_info_t *mbinfo = (multiboot_info_t*) to_virtual(ebx);
	mbinfo->mods_addr = to_virtual(mbinfo->mods_addr);
	mbinfo->mmap_addr = to_virtual(mbinfo->mmap_addr);
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
	/* TESTS */
	
	multiboot_module_t *module = (multiboot_module_t*) mbinfo->mods_addr;
	//module++;

	process_t *process = load_module(module);

	if (mbinfo->mods_count == 2 && mbinfo->flags & (1 << 3)) {
		//printf("Start: %#.8X\nEnd:   %#.8X\nString: %s\n", module->mod_start, module->mod_end, module->cmdline + 0xC0000000);
		//printf("Process:        %#.8X\n", process);
		//printf("Page Directory: %#.8X\n", &process->page_directory);
		//printf("Kernel PT:      %#.8X\n", KERNEL_PAGE_TABLE_PHYSICAL_ADDRESS);
		load_program(get_physical_address(&process->page_directory));	
	}		

	//puts((void*) 0x1000);
	//for (size_t i = 0; i < num_code_pages; i++)
	//	printf("Code Page Table [%d]: %#.8X\n", i, process->code_page_table.entries[i]);

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