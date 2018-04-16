#ifndef _KERNEL_PROCESS_H
#define _KERNEL_PROCESS_H

#include <kernel/page.h>
#include <stdbool.h>
#include <stdint.h>

struct _cpu_state {
    uint32_t ebp;
    uint32_t edi;
    uint32_t esi;
    uint32_t edx;
    uint32_t ecx;
    uint32_t ebx;
    uint32_t eax;
} __attribute__((packed));

typedef struct _cpu_state cpu_state_t;

struct _stack_state {
    uint32_t eip;
    uint32_t cs;
    uint32_t eflags;
    uint32_t esp;
    uint32_t ss;
} __attribute__((packed));

typedef struct _stack_state stack_state_t;

struct _process_state {
    cpu_state_t cpu;
    stack_state_t stack;
} __attribute__((packed));

typedef struct _process_state process_state_t;

struct _process_page_table {
    page_table_t *page_table;
    page_t *(pages[MAX_PAGE_TABLE_ENTRIES]);
};

typedef struct _process_page_table process_page_table_t;

struct _process_memory {
	page_directory_t *page_directory;
    process_page_table_t *(page_tables[MAX_PAGE_DIRECTORY_ENTRIES]);
};

typedef struct _process_memory process_memory_t;

struct _process {
    process_state_t process_state;
    process_memory_t *process_memory;
    bool ring0;
    uint32_t pid;
    struct _process *next;
    struct _process *prev;
};

typedef struct _process process_t;

process_t *add_process(process_memory_t *process_memory, uint32_t argc, char *argv[], uint32_t envc, char *envp[]);
void run_process(process_t *process);

#endif