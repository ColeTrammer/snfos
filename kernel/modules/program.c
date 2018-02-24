void main() {
    int a = 1;

    for (int i = 0; i < 1024; i++) {
      a += i;
    }

    __asm__("mov %0, %%eax" : : "r"(a));

    return;
}