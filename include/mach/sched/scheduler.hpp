#ifndef NEBULA_SCHEDULER_SCHEDULER_HPP
#define NEBULA_SCHEDULER_SCHEDULER_HPP

#include <stddef.h>
#include <stdint.h>
#include <mach/sched/process.hpp>
#include <mach/arch/interrupts.hpp>

namespace nebula {
namespace scheduler {

using nebula::process::Thread;
using nebula::process::Process;
using nebula::process::ThreadState;

static const size_t MAX_THREADS = 16;

/**
 * @brief Preemptive Round-Robin Scheduler Manager
 */
class Scheduler {
private:
    static Thread m_threads[MAX_THREADS];
    static size_t m_thread_count;
    static size_t m_current_thread_index;
    static bool m_initialized;

public:
    /**
     * @brief Initialize Scheduler and register main kernel thread (TID 0)
     */
    static void init();

    /**
     * @brief Create a new background kernel thread with 16 KiB stack
     */
    static Thread* create_kernel_thread(void (*entry_point)(), const char* name = nullptr);

    /**
     * @brief Timer Tick handler (IRQ 0 / 100 Hz Time Slicing)
     * @param regs CPU registers frame passed by ISR stub
     * @return New thread's stack pointer (ESP) to switch to
     */
    static uint32_t handle_timer_tick(nebula::arch::x86_64::registers_t* regs);

    /**
     * @brief Manually yield CPU time slice
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
