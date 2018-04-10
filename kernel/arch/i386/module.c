#include <kernel/module.h>
#include <kernel/page.h>
#include <kernel/heap.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <stddef.h>
#include <grub/multiboot.h>

process_memory_t *load_module(multiboot_module_t *module) {
    process_memory_t *process = malloc_pages(sizeof(process_memory_t) / PAGE_SIZE);
	//printf("Process Addr: %#.8X\n", process);
	memset(process, sizeof(process_memory_t), 0);
	process->page_directory.entries[LAST_PAGE_DIRECTORY_INDEX] = get_physical_address(&process->page_directory) | 0x03;
	process->page_directory.entries[KERNEL_VIRTUAL_BASE >> 22] = KERNEL_PAGE_TABLE_PHYSICAL_ADDRESS | 0x03;
	process->page_directory.entries[0] = get_physical_address(&process->code_page_table) | 0x07;
	process->page_directory.entries[(KERNEL_VIRTUAL_BASE >> 22) - 1] = get_physical_address(&process->stack_page_table) | 0x07;
	process->page_directory.entries[LAST_PAGE_DIRECTORY_INDEX - 1] = get_physical_address(&process->kernel_stack_page_table) | 0x03;

	process->stack_page_table.entries[LAST_PAGE_TABLE_INDEX] = get_physical_address(malloc_pages(1)) | 0x07;
	process->kernel_stack_page_table.entries[LAST_PAGE_TABLE_INDEX] = get_physical_address(malloc_pages(1)) | 0x03;

    size_t num_code_pages = NUM_PAGES(module->mod_start, module->mod_end);
	//printf("Mod Start: %#.8X\n", module->mod_start);
	//printf("Number of Code Pages: %d\n", num_code_pages);
	uint8_t *temp_pages = malloc_pages(num_code_pages);
	//printf("Temp Pages Virt Addr: %#.8X\n", temp_pages);
	//printf("Temp Pages Phys Addr: %#.8X\n", get_physical_address(temp_pages));
	//printf("Used: %d %d\n", check_used((void*) module->mod_start), check_used((void*) (module->mod_start + PAGE_SIZE)));
	for (size_t i = 0; i < num_code_pages; i++) {
		free_physical_page(get_physical_address(&temp_pages[PAGE_SIZE * i]));
		map_page(((uint32_t) temp_pages) + (PAGE_SIZE * i), (module->mod_start + (PAGE_SIZE * i)) & 0xFFFFF000, 0x03);
		//printf("Phys Addr Mapped [%d]: %#.8X\n", i, ((module->mod_start + (PAGE_SIZE * i)) & 0xFFFFF000) - 0xC0000000);
	}

	//for (size_t i = 0x000; i < 0x008; i++)
	//	printf("%s%#.2X\n",  i % 4 == 0 ? "\n" : "", temp_pages[i]);
	//puts("");

	uint8_t *program_code = malloc_pages(num_code_pages);
	for (size_t i = 0; i < num_code_pages; i++) { 
		process->code_page_table.entries[i] = get_physical_address(&program_code[i * PAGE_SIZE]) | 0x07;
	}

	memcpy(program_code, temp_pages, module->mod_end - module->mod_start);

	//for (size_t i = 0x1000; i < 0x1008; i++)
	//	printf("%s%#.2X\n",  i % 4 == 0 ? "\n" : "", program_code[i]);
	
	free(temp_pages);
	return process;
}