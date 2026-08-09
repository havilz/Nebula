#ifndef NEBULA_PROCESS_PROCESS_HPP
#define NEBULA_PROCESS_PROCESS_HPP

#include <stddef.h>
#include <stdint.h>

namespace nebula {
namespace process {

/**
 * @brief Thread execution states
 */
enum class ThreadState {
    EMBRYO,
    READY,
    RUNNING,
    WAITING,
    TERMINATED
};

/**
 * @brief CPU Register Context Structure for Thread Context Switch
 */
struct __attribute__((packed)) cpu_context_t {
    uint32_t edi;
    uint32_t esi;
    uint32_t ebp;
    uint32_t esp;
    uint32_t ebx;
    uint32_t edx;
    uint32_t ecx;
    uint32_t eax;
    uint32_t eip;
    uint32_t eflags;
};

/**
 * @brief Thread Control Block (TCB)
 */
struct Thread {
    uint32_t tid;
    char name[32];
    ThreadState state;
    uintptr_t stack_base;
    uintptr_t stack_size;
    cpu_context_t context;
    Thread* next;
};

/**
 * @brief Process Control Block (PCB)
 */
struct Process {
    uint32_t pid;
    char name[32];
    uintptr_t page_directory;
    Thread* threads;
    Process* next;
};

} // namespace process
} // namespace nebula

#endif // NEBULA_PROCESS_PROCESS_HPP
