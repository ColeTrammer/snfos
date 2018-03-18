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
    asm("mov %0, %%eax" : : "r" (s) : "eax");
    asm("int $0x80");
}

void main(void) {
    //int a = 2;

    char *s = "Program 1";

    //s[1] = '0';

    //inc_a(&a);
    //while (1) {
      //inc_a(&a);
      //    asm("int $0x80");
      //    asm("mov %%eax, %0" : "=r"(a));          
      //__asm__("mov %0, %%ebx" : : "r"(a));
    //}
    for (int i = 0; i < 10; i++) {
      print(s);
    }
}  