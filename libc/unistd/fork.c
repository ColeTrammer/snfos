#include <unistd.h>

pid_t fork() {
  unsigned int res;
  asm("mov $4, %%eax\n"\
      "int $0x80\n"\
      "mov %%ecx, %0\n"
      : "=m"(res) : : "eax", "ecx");
  return res;
} 