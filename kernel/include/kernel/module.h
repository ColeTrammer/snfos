#ifndef _KERNEL_MODULE_H
#define _KERNEL_MODULE_H

#include <kernel/process.h>
#include <grub/multiboot.h>

process_memory_t *load_module(multiboot_module_t *module);

#endif