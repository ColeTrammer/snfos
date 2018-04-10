#ifndef _KERNEL_MODULE_H
#define _KERNEL_MODULE_H

#include <kernel/page.h>
#include <grub/multiboot.h>

typedef struct _process {
		page_directory_t page_directory;
		page_table_t code_page_table;
		page_table_t stack_page_table;
		page_table_t kernel_stack_page_table;
	} __attribute__((packed)) process_memory_t;

process_memory_t *load_module(multiboot_module_t *module);

#endif