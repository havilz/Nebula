#ifndef NEBULA_SCHEDULER_SCHEDULER_HPP
#define NEBULA_SCHEDULER_SCHEDULER_HPP

#include <stddef.h>
#include <stdint.h>
#include "../process/process.hpp"
#include "../arch/x86_64/interrupts.hpp"

namespace nebula {
namespace scheduler {

using nebula::process::Thread;
using nebula::process::ThreadState;
using nebula::process::cpu_context_t;

static const size_t MAX_THREADS = 64;

/**
 * @brief Preemptive Round-Robin Scheduler Class
 */
class Scheduler {
private:
    static Thread m_threads[MAX_THREADS];
    static size_t m_thread_count;
    static size_t m_current_thread_index;
    static bool m_initialized;

public:
    /**
     * @brief Initialize Preemptive Round-Robin Scheduler and register timer hook
     */
    static void init();

    /**
     * @brief Create a new kernel thread
     * @param entry_point Pointer to thread entry function
     * @param name Name identifier for thread
     * @return Pointer to created Thread, or nullptr if thread limit reached
     */
    static Thread* create_kernel_thread(void (*entry_point)(), const char* name);

    /**
     * @brief Preemptive scheduling tick handler called by PIT IRQ 0
     * @param regs CPU registers frame passed by ISR stub
     */
    static void handle_timer_tick(nebula::arch::x86_64::registers_t* regs);

    /**
     * @brief Voluntarily yield current thread execution to next ready thread
     */
    static void yield();

    /**
     * @brief Get currently running thread
     */
    static Thread* get_current_thread();
};

} // namespace scheduler
} // namespace nebula

extern "C" {
    void scheduler_init();
    void scheduler_yield();
}

#endif // NEBULA_SCHEDULER_SCHEDULER_HPP
