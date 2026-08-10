#ifndef LIBNEBULA_STDIO_H
#define LIBNEBULA_STDIO_H

#include <stddef.h>
#include <stdarg.h>

#ifdef __cplusplus
extern "C" {
#endif

int putchar(int c);
int puts(const char *s);
int printf(const char *format, ...);
int sprintf(char *str, const char *format, ...);

#ifdef __cplusplus
}
#endif

#endif // LIBNEBULA_STDIO_H
