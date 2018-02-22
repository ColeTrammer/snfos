.global _start
_start:
  mov $0xDEADBEEF, %eax
  int $0
  ret
