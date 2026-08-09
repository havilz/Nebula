#include <unistd.h>
#include <stdint.h>

static inline int __syscall3(int num, int a1, int a2, int a3) {
    int ret;
    __asm__ __volatile__ (
        "int $0x80"
        : "=a"(ret)
        : "a"(num), "b"(a1), "c"(a2), "d"(a3)
        : "memory"
    );
    return ret;
}

ssize_t write(int fd, const void *buf, size_t count) {
    return __syscall3(2, fd, (int)(uintptr_t)buf, (int)count); // SYS_WRITE = 2
}

ssize_t read(int fd, void *buf, size_t count) {
    return __syscall3(1, (int)(uintptr_t)buf, 0, (int)count); // SYS_READ = 1
}

int close(int fd) {
    return __syscall3(4, fd, 0, 0); // SYS_CLOSE = 4
}

pid_t fork(void) {
    return __syscall3(2, 0, 0, 0); // SYS_FORK = 2
}

int execve(const char *filename, char *const argv[], char *const envp[]) {
    (void)argv; (void)envp;
    return __syscall3(11, (int)(uintptr_t)filename, 0, 0); // SYS_EXEC = 11
}

void yield(void) {
    __syscall3(5, 0, 0, 0); // SYS_YIELD = 5
}

void _exit(int status) {
    __syscall3(6, status, 0, 0); // SYS_EXIT = 6
    while(1);
}
