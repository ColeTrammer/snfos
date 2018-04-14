#ifndef _STDIO_H
#define _STDIO_H 1

#include <sys/cdefs.h>
#include <stddef.h>
#include <stdarg.h>

#define EOF (-1)

#define SEEK_SET 0xAAAABBBB

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _iobuf
{
    char*   _ptr;
    int _cnt;
    char*   _base;
    int _flag;
    int _file;
    int _charbuf;
    int _bufsiz;
    char*   _tmpfname;
} FILE;

extern FILE *stderr;

int fflush(FILE *stream);
int fprintf(FILE *stream, const char *__restrict, ...);

int fclose(FILE *stream);
FILE *fopen(const char *pathname, const char *mode);
size_t fread(void *ptr, size_t size, size_t nmemb, FILE *stream);
int fseek(FILE *stream, long offset, int whence);
long ftell(FILE *stream);
size_t fwrite(const void *ptr, size_t size, size_t nmemb, FILE *stream);
void setbuf(FILE *stream, char *buf);

int vfprintf(FILE *stream, const char *format, va_list ap);

int printf(const char *__restrict, ...);
int putchar(int);
int puts(const char *s);

#ifdef __cplusplus
}
#endif

#endif
