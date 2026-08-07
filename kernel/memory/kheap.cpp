/**
 * @file kheap.cpp
 * @brief Global C++ dynamic memory operators (new/delete) for freestanding kernel
 * @author Nebula OS Team
 */

#include "../../include/kernel/memory/heap.hpp"

// Global single object allocation
void* operator new(size_t size) {
    return kmalloc(size);
}

// Global array object allocation
void* operator new[](size_t size) {
    return kmalloc(size);
}

// Global single object deallocation
void operator delete(void* ptr) noexcept {
    kfree(ptr);
}

// Global array object deallocation
void operator delete[](void* ptr) noexcept {
    kfree(ptr);
}

// Sized single object deallocation (C++14/C++17)
void operator delete(void* ptr, size_t) noexcept {
    kfree(ptr);
}

// Sized array object deallocation (C++14/C++17)
void operator delete[](void* ptr, size_t) noexcept {
    kfree(ptr);
}

// Placement new operator
void* operator new(size_t, void* ptr) noexcept {
    return ptr;
}

// Placement array new operator
void* operator new[](size_t, void* ptr) noexcept {
    return ptr;
}
