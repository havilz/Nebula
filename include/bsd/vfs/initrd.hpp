#ifndef NEBULA_FS_INITRD_HPP
#define NEBULA_FS_INITRD_HPP

#include <stddef.h>
#include <stdint.h>
#include <bsd/vfs/vfs.hpp>

namespace nebula {
namespace fs {

static const uint32_t INITRD_MAGIC = 0xBF00BF00;
static const size_t MAX_INITRD_FILES = 16;

/**
 * @brief Initrd File Entry Structure inside RAM Disk Image
 */
struct __attribute__((packed)) initrd_file_header_t {
    uint32_t magic;
    char name[64];
    uint32_t offset;
    uint32_t length;
};

/**
 * @brief Initrd RAM Disk Header Structure
 */
struct __attribute__((packed)) initrd_header_t {
    uint32_t nfiles;
};

/**
 * @brief Initrd Filesystem Driver Class
 */
class Initrd {
private:
    static vnode_t m_initrd_dev;
    static vnode_t m_initrd_root;
    static vnode_t m_file_nodes[MAX_INITRD_FILES];
    static initrd_file_header_t m_file_headers[MAX_INITRD_FILES];
    static size_t m_file_count;

    static vnode_operations_t m_initrd_ops;

public:
    /**
     * @brief Initialize Initrd RAM Disk and mount root node at /initrd/
     * @param location Memory location of built-in initrd image
     * @return Pointer to initrd root vnode
     */
    static vnode_t* init(uintptr_t location);

    /**
     * @brief Read callback for Initrd files
     */
    static uint32_t read(vnode_t* node, uint32_t offset, uint32_t size, uint8_t* buffer);

    /**
     * @brief Directory search callback for Initrd
     */
    static vnode_t* finddir(vnode_t* node, const char* name);
};

} // namespace fs
} // namespace nebula

extern "C" {
    nebula::fs::vnode_t* initrd_init(uintptr_t location);
}

#endif // NEBULA_FS_INITRD_HPP
