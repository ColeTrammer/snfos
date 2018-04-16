#include <unistd.h>
#include <stdio.h>

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

void yield() {
  asm("mov $1, %%eax\n"\
      "int $0x80\n"
      : : : "eax");
}

pid_t get_pid() {
  unsigned int pid;
  asm("mov $3, %%eax\n"\
      "int $0x80\n"\
      "mov %%eax, %0\n"
      : "=m"(pid) : : "eax");
  return pid;
}

int main(int argc, char *argv[]) {
    printf("Argc: %u\n", argc);
    printf("Argv[0]: %s\n", argv[0]);

    pid_t res = fork();
    if (res == -1) {
      puts("error");
      return 1;
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

    for (int i = 0; i < 1; i++) {
      puts(s);
      yield();
    }
    return 0;
}  