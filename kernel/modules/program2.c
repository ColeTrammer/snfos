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

void main(void) {
    //int a = 2;
/*
    char *s = "PID: _";

    //s[1] = '0';

    //inc_a(&a);
    //while (1) {
      //inc_a(&a);
      //    asm("int $0x80");
      //    asm("mov %%eax, %0" : "=r"(a));          
      //__asm__("mov %0, %%ebx" : : "r"(a));
    //}
    s[5] = get_pid() + '0';
    for (int i = 0; i < 10; i++) {
      print(s);
//      if (i % 2 == 1)
        yield();

      //asm("movl (0xC001000), %%edx" : : : "edx");
    }
    */
}  