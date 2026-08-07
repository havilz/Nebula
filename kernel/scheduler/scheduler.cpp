/**
 * @file scheduler.cpp
 * @brief Preemptive Round-Robin Scheduler implementation for Nebula OS
 * @author Nebula OS Team
 */

#include "../../include/kernel/scheduler/scheduler.hpp"
#include "../../include/kernel/drivers/serial.hpp"
#include "../../include/kernel/memory/heap.hpp"

namespace nebula {
namespace scheduler {

Thread Scheduler::m_threads[MAX_THREADS];
size_t Scheduler::m_thread_count = 0;
size_t Scheduler::m_current_thread_index = 0;
bool Scheduler::m_initialized = false;

static void str_copy(char* dest, const char* src, size_t max_len) {
    if (!dest || !src) return;
    size_t i = 0;
    while (src[i] != '\0' && i < max_len - 1) {
        dest[i] = src[i];
        i++;
    }
    dest[i] = '\0';
}

void Scheduler::init() {
    m_thread_count = 0;
    m_current_thread_index = 0;
    m_initialized = true;

    // Create Main Kernel Thread (TID 0)
    Thread* main_thread = &m_threads[0];
    main_thread->tid = 0;
    str_copy(main_thread->name, "KernelMain", sizeof(main_thread->name));
    main_thread->state = ThreadState::RUNNING;
    main_thread->stack_base = 0;
    main_thread->stack_size = 0;
    main_thread->next = nullptr;
    m_thread_count = 1;
}

Thread* Scheduler::create_kernel_thread(void (*entry_point)(), const char* name) {
    if (!m_initialized || m_thread_count >= MAX_THREADS || entry_point == nullptr) {
        return nullptr;
    }

    size_t index = m_thread_count;
    Thread* thread = &m_threads[index];

    // Allocate 16 KiB kernel stack for new thread
    size_t stack_size = 16384;
    void* stack = nebula::memory::KernelHeap::kmalloc(stack_size);
    if (stack == nullptr) {
        return nullptr;
    }

    uint8_t* stack_top = static_cast<uint8_t*>(stack) + stack_size;

    // Build initial registers_t stack frame for CPU iret entry
    stack_top -= sizeof(nebula::arch::x86_64::registers_t);
    stack_top = reinterpret_cast<uint8_t*>(reinterpret_cast<uintptr_t>(stack_top) & ~0xF);

    nebula::arch::x86_64::registers_t* frame = reinterpret_cast<nebula::arch::x86_64::registers_t*>(stack_top);
    uint8_t* fptr = reinterpret_cast<uint8_t*>(frame);
    for (size_t i = 0; i < sizeof(nebula::arch::x86_64::registers_t); i++) fptr[i] = 0;

    frame->cs = 0x08;
    frame->ds = 0x10;
    frame->eip = reinterpret_cast<uint32_t>(entry_point);
    frame->eflags = 0x202; // IF enabled

    thread->tid = static_cast<uint32_t>(index);
    str_copy(thread->name, name ? name : "KernelTask", sizeof(thread->name));
    thread->state = ThreadState::READY;
    thread->stack_base = reinterpret_cast<uintptr_t>(stack);
    thread->stack_size = stack_size;

    thread->context.eip = reinterpret_cast<uint32_t>(entry_point);
    thread->context.esp = reinterpret_cast<uint32_t>(frame);
    thread->context.eflags = 0x202;

    m_thread_count++;
    return thread;
}

uint32_t Scheduler::handle_timer_tick(nebula::arch::x86_64::registers_t* regs) {
    if (!m_initialized || m_thread_count <= 1 || regs == nullptr) {
        return reinterpret_cast<uint32_t>(regs);
    }

    // Save current thread stack pointer (top of stack with registers_t frame!)
    Thread* current = &m_threads[m_current_thread_index];
    current->context.esp = reinterpret_cast<uint32_t>(regs);

    if (current->state == ThreadState::RUNNING) {
        current->state = ThreadState::READY;
    }

    // Pick next READY thread (Round-Robin)
    size_t next_index = m_current_thread_index;
    do {
        next_index = (next_index + 1) % m_thread_count;
        if (m_threads[next_index].state == ThreadState::READY) {
            break;
        }
    } while (next_index != m_current_thread_index);

    m_current_thread_index = next_index;
    Thread* next_thread = &m_threads[m_current_thread_index];
    next_thread->state = ThreadState::RUNNING;

    // Return new thread's stack pointer for irq_common_stub mov esp, eax!
    return next_thread->context.esp;
}

void Scheduler::yield() {
    asm volatile ("int $32"); // Trigger IRQ 0 timer tick
}

Thread* Scheduler::get_current_thread() {
    return &m_threads[m_current_thread_index];
}

} // namespace scheduler
} // namespace nebula

extern "C" {
    void scheduler_init() {
        nebula::scheduler::Scheduler::init();
    }

    void scheduler_yield() {
        nebula::scheduler::Scheduler::yield();
    }
}
