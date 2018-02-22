#ifndef IO_H
#define IO_H

#include <stdint.h>

static inline void outb(uint16_t port, unsigned char data) {
	__asm__("outb %0, %1" : : "a" (data), "Nd" (port));
}

static inline uint8_t inb(uint16_t port) {
	uint8_t res;
	__asm__("inb %1, %0" : "=a" (res) : "Nd" (port));
	return res;
}

static inline void io_wait(void) {
	__asm__ volatile ("outb %%al, $0x80" : : "a"(0));
}

#endif