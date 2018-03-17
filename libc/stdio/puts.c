#include <stdio.h>

int puts(const char* string) {
	return printf("%#.8X: %s\n", string, string);
}
