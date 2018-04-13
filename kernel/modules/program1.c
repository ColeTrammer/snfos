//#include <stdint.h>

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

unsigned int get_pid() {
  unsigned int pid;
  asm("mov $3, %%eax\n"\
      "int $0x80\n"\
      "mov %%ecx, %0\n"
      : "=m"(pid) : : "eax", "ecx");
  return pid;
}

unsigned int fork() {
  unsigned int res;
  asm("mov $4, %%eax\n"\
      "int $0x80\n"\
      "mov %%ecx, %0\n"
      : "=m"(res) : : "eax", "ecx");
  return res;
}

void main(void) {
    unsigned int res = fork();
    if (res == -1) {
      print("Error");
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