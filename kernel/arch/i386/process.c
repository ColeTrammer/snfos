#include <kernel/process.h>
#include "sysreturn.h"
#include <stdio.h>
#include <stdlib.h>
#include <kernel/page.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

static process_t *first;
static process_t *last;
static process_t *current;
static uint32_t   pid_inc = 1;

/*
static inline uint32_t get_eflags() {
    uint32_t eflags;
    asm("pushf\n"\
        "pop %%ecx\n"
        : "=c"(eflags));
    return eflags;
}
*/

static inline void load_cr3(uint32_t cr3) {
    asm("mov %0, %%edx\n"\
        "mov %%edx, %%cr3\n"
        : : "m"(cr3) : "edx");
}

process_t *add_process(process_memory_t *process_memory) {
    process_t *new_process;
    if (!first) {
        new_process = first = last = malloc(sizeof(process_t));
        first->prev = NULL;
    } else {
        new_process = malloc(sizeof(process_t));
    }
    new_process->process_memory = process_memory;
    new_process->ring0 = false;
    new_process->pid = pid_inc++;

    new_process->process_state.stack.ss     = 0x20 | 0x03;
    new_process->process_state.stack.esp    = KERNEL_VIRTUAL_BASE - 4;
    new_process->process_state.stack.eflags = 0x10 | 0x200;
    new_process->process_state.stack.cs     = 0x18 | 0x03;
    new_process->process_state.stack.eip    = 0x0;

    new_process->prev = last;
    new_process->next = first;
    first->prev       = new_process;
    last->next        = new_process;
    last = new_process;
    return new_process;
}

void run_process(process_t *process) {
    current = process;
    load_cr3(get_physical_address(process->process_memory->page_directory));
    sys_return(process->process_state);
}

void yield(process_state_t process_state) {
    current->process_state = process_state;
    run_process(current->next);
    //sys_return(process_state);
    //printf("Eip: %#.8X  Esp: %#.8X  Eax: %#.8X\n", process_state.stack.eip, process_state.stack.esp, process_state.cpu.eax);
    //printf("Eflags: %#.8X\n", process_state.stack.eflags);
    //sys_return(process_state);
    //printf("Eip: %#.8X  Esp: %#.8X  Eax: %#.8X\n", stack.eip, stack.esp, cpu.eax);
    //printf("Eip: %#.8X\nEdx: %#.8X\nEcx: %#.8X\nEbx: %#.8X\nEax: %#.8X\nEsi: %#.8X\nEdi: %#.8X\nEbp: %#.8X\nCs:  %#.2X\nEsp: %#.8X\nSs:  %#.2X\nEflags: %#.8X\n", stack.eip, cpu.edx, cpu.ecx, cpu.ebx, cpu.eax, cpu.esi, cpu.edi, cpu.ebp, stack.cs, stack.esp, stack.ss, stack.eflags);
    //print_edx();
}

void handle_exit(/*process_state_t process_state*/) {
    if (current == first && current == last) {
        asm("sti");
        while (1);
    } 
    
    if (current == first) {
        first = current->next;
        //last->next = first;
        //first->prev = last;
    } 

    if (current == last) {
        last = current->prev;
    }
    
    current->prev->next = current->next;
    current->next->prev = current->prev;
    process_t *next = current->next;

    load_cr3(get_physical_address(next->process_memory->page_directory));

    free(current->process_memory->page_directory);
    for (size_t i = 0; i < MAX_PAGE_DIRECTORY_ENTRIES; i++) {
        if (current->process_memory->page_tables[i]) {
            free(current->process_memory->page_tables[i]->page_table);
            for (size_t j = 0; j < MAX_PAGE_TABLE_ENTRIES; j++) {
                free(current->process_memory->page_tables[i]->pages[j]);
            }
            free(current->process_memory->page_tables[i]);
        }
    }
    free(current->process_memory);
    free(current);

    run_process(next);
}

void fork(process_state_t process_state) {
    process_memory_t *new_process_memory = calloc(1, sizeof(process_memory_t));
    new_process_memory->page_directory = malloc_pages(1);
    memset(new_process_memory->page_directory, 0, sizeof(page_directory_t));
    for (size_t i = 0; i < MAX_PAGE_DIRECTORY_ENTRIES; i++) {
        if (current->process_memory->page_tables[i]) {
            new_process_memory->page_tables[i] = calloc(1, sizeof(process_page_table_t));
            new_process_memory->page_tables[i]->page_table = malloc_pages(1);
            memset(new_process_memory->page_tables[i]->page_table, 0, sizeof(page_table_t));
            for (size_t j = 0; j < MAX_PAGE_TABLE_ENTRIES; j++) {
                if (current->process_memory->page_tables[i]->pages[j]) {
                    new_process_memory->page_tables[i]->pages[j] = malloc_pages(1);
                    memcpy(new_process_memory->page_tables[i]->pages[j], current->process_memory->page_tables[i]->pages[j], sizeof(page_t));
                    new_process_memory->page_tables[i]->page_table->entries[j] = get_physical_address(new_process_memory->page_tables[i]->pages[j]) | (current->process_memory->page_tables[i]->page_table->entries[j] & 0xFF);
                }
            }
            new_process_memory->page_directory->entries[i] = get_physical_address(new_process_memory->page_tables[i]->page_table) | (current->process_memory->page_directory->entries[i] & 0xFF);
        }
    }
    new_process_memory->page_directory->entries[LAST_PAGE_DIRECTORY_INDEX] = get_physical_address(new_process_memory->page_directory) | 0x03;
	new_process_memory->page_directory->entries[KERNEL_VIRTUAL_BASE >> 22] = KERNEL_PAGE_TABLE_PHYSICAL_ADDRESS | 0x03;
    process_t *new_process = add_process(new_process_memory);
    memcpy(&new_process->process_state, &process_state, sizeof(process_state_t));
    new_process->process_state.cpu.ecx = 0;
    process_state.cpu.ecx = new_process->pid;
    sys_return(process_state);
}

void get_pid(process_state_t process_state) {
    process_state.cpu.ecx = current->pid;
    sys_return(process_state);
}