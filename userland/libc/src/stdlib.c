#include <stdlib.h>
#include <unistd.h>

void exit(int status) {
    _exit(status);
}

int atoi(const char* str) {
    if (!str) return 0;
    int res = 0;
    int sign = 1;
    int i = 0;
    if (str[0] == '-') {
        sign = -1;
        i++;
    }
    for (; str[i] != '\0'; ++i) {
        if (str[i] < '0' || str[i] > '9') break;
        res = res * 10 + str[i] - '0';
    }
    return sign * res;
}

int abs(int j) {
    return (j < 0) ? -j : j;
}

void* malloc(size_t size) {
    (void)size;
    return NULL;
}

void free(void* ptr) {
    (void)ptr;
}
