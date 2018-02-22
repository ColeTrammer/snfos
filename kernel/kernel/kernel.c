#include <stdio.h>

#include <kernel/tty.h>
#include <kernel/input.h>
#include <grub/multiboot.h>
#include <stdbool.h>
#include <kernel/page.h>

typedef void (*call_module_t)(void);

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
	
	/* TESTS */

	multiboot_module_t *module = (multiboot_module_t*) mbinfo->mods_addr;
	call_module_t start_program = (call_module_t) module->mod_start;
	uint32_t virt_page_address = ((uint32_t) module->mod_start) & 0xFFFFF000;
	uint32_t phys_page_address = virt_page_address - 0xC0000000;

	uint32_t page_directory_offset = virt_page_address >> 22;
	uint32_t page_table_offset = (virt_page_address >> 12) & 0x03FF;//gets first 10 digits

	
	uint32_t *page_table = (uint32_t*) ((1023 << 22) | (page_directory_offset << 12));
	page_table[page_table_offset] = phys_page_address | 0x03;

	if (mbinfo->mods_count == 1 && mbinfo->flags & (1 << 3)) {
		start_program();
	}
	//printf("Start: %#.8X\nEnd:   %#.8X\nString: %s\n", module->mod_start, module->mod_end, module->cmdline);
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