/**
 * @file kheap.cpp
 * @brief Global C++ operator new and delete overloads for Nebula OS
 * @author Nebula OS Team
 */

#include <mach/vm/heap.hpp>

void* operator new(size_t size) {
    return kmalloc(size);
}

void* operator new[](size_t size) {
    return kmalloc(size);
}

void operator delete(void* p) noexcept {
    kfree(p);
}

void operator delete[](void* p) noexcept {
    kfree(p);
}

void operator delete(void* p, size_t) noexcept {
    kfree(p);
}

void operator delete[](void* p, size_t) noexcept {
    kfree(p);
}
