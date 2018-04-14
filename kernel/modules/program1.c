#include <unistd.h>

/*void inc_a(int *a) {
  *a = *a + 1;
  if (*a == 0) {
    *a = 1;
  } else if (*a == -123489) {
    *a = -1;
  }
  *a >>= 4;
  (*a)++;
  *a <<= 4;
}*/

void print(const char *s) {
    asm("mov $0, %%eax\n"\
        "mov %0, %%ebx\n"\
        "int $0x80\n"
         : : "r" (s) : "eax", "ebx");
}

void yield() {
  asm("mov $1, %%eax\n"\
      "int $0x80\n"
      : : : "eax");
}

pid_t get_pid() {
  unsigned int pid;
  asm("mov $3, %%eax\n"\
      "int $0x80\n"\
      "mov %%ecx, %0\n"
      : "=m"(pid) : : "eax", "ecx");
  return pid;
}

void main(void) {
    pid_t res = fork();
    if (res == -1) {
      print("error");
      return;
    } 
    
    char *s = "";
    if (res == 0) {
      s = "pid: _ :: child";
      s[5] = get_pid() + '0';
    } else {
      s = "pid: _ :: parent of _";
      s[5] = get_pid() + '0';
      s[20] = res + '0';
    }

    for (int i = 0; i < 10; i++) {
      print(s);
      yield();
    }
}  