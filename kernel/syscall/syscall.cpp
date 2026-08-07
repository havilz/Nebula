/**
 * @file syscall.cpp
 * @brief System Call Interrupt 0x80 Dispatcher implementation for Nebula OS
 * @author Nebula OS Team
 */

#include "../../include/kernel/syscall/syscall.hpp"
#include "../../include/kernel/drivers/serial.hpp"
#include "../../include/kernel/drivers/console.hpp"
#include "../../include/kernel/fs/vfs.hpp"
#include "../../include/kernel/scheduler/scheduler.hpp"

namespace nebula {
namespace syscall {

void Syscall::init() {
    // Register INT 0x80 (Vector 128) handler
    nebula::arch::x86_64::register_interrupt_handler(128, Syscall::handle_syscall);
    nebula::drivers::Serial::write_string("[SYSCALL] INT 0x80 System Call Dispatcher Registered\n");
}

void Syscall::handle_syscall(nebula::arch::x86_64::registers_t* regs) {
    if (regs == nullptr) return;

    uint32_t syscall_num = regs->eax;
    uint32_t arg1 = regs->ebx;
    uint32_t arg2 = regs->ecx;
    uint32_t arg3 = regs->edx;

    switch (syscall_num) {
        case SYS_WRITE: {
            // arg1: fd (1=stdout, 2=stderr), arg2: const char* str, arg3: count
            const char* str = reinterpret_cast<const char*>(arg2);
            if (str != nullptr) {
                nebula::drivers::Serial::write_string(str);
            }
            regs->eax = arg3;
            break;
        }

        case SYS_READ: {
            // arg1: vnode_t* node, arg2: offset, arg3: buffer pointer
            nebula::fs::vnode_t* node = reinterpret_cast<nebula::fs::vnode_t*>(arg1);
            uint8_t* buf = reinterpret_cast<uint8_t*>(arg3);
            if (node && buf) {
                regs->eax = nebula::fs::VFS::read(node, arg2, 256, buf);
            } else {
                regs->eax = 0;
            }
            break;
        }

        case SYS_YIELD: {
            nebula::scheduler::Scheduler::yield();
            regs->eax = 0;
            break;
        }

        case SYS_EXIT: {
            nebula::drivers::Serial::write_string("[USERLAND] Process exited via SYS_EXIT\n");
            regs->eax = 0;
            break;
        }

        default: {
            nebula::drivers::Serial::write_string("[SYSCALL] Unknown System Call Number Received!\n");
            regs->eax = static_cast<uint32_t>(-1);
            break;
        }
    }
}

} // namespace syscall
} // namespace nebula

extern "C" {
    void syscall_init() {
        nebula::syscall::Syscall::init();
    }
}
