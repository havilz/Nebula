#include <stdio.h>
#include <unistd.h>
#include <string.h>

int putchar(int c) {
    char ch = (char)c;
    write(STDOUT_FILENO, &ch, 1);
    return c;
}

int puts(const char *s) {
    if (!s) return -1;
    size_t len = strlen(s);
    write(STDOUT_FILENO, s, len);
    putchar('\n');
    return len + 1;
}

int printf(const char *format, ...) {
    if (!format) return 0;
    va_list args;
    va_start(args, format);

    int count = 0;
    for (const char* p = format; *p != '\0'; p++) {
        if (*p != '%') {
            putchar(*p);
            count++;
            continue;
        }
        p++;
        if (*p == 's') {
            const char* s = va_arg(args, const char*);
            if (!s) s = "(null)";
            puts(s);
        } else if (*p == 'c') {
            char c = (char)va_arg(args, int);
            putchar(c);
            count++;
        } else if (*p == '%') {
            putchar('%');
            count++;
        }
    }

    va_end(args);
    return count;
}
