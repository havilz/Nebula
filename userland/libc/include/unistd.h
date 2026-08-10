#ifndef LIBNEBULA_UNISTD_H
#define LIBNEBULA_UNISTD_H

#include "sys/types.h"
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

#define STDIN_FILENO  0
#define STDOUT_FILENO 1
#define STDERR_FILENO 2

ssize_t write(int fd, const void *buf, size_t count);
ssize_t read(int fd, void *buf, size_t count);
int     close(int fd);
pid_t   fork(void);
int     execve(const char *filename, char *const argv[], char *const envp[]);
void    yield(void);
void    _exit(int status);

#ifdef __cplusplus
}
#endif

#endif // LIBNEBULA_UNISTD_H
