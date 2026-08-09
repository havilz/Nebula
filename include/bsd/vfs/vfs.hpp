#ifndef NEBULA_FS_VFS_HPP
#define NEBULA_FS_VFS_HPP

#include <stddef.h>
#include <stdint.h>

namespace nebula {
namespace fs {

static const uint32_t VFS_FILE        = 0x01;
static const uint32_t VFS_DIRECTORY   = 0x02;
static const uint32_t VFS_CHARDEVICE  = 0x03;
static const uint32_t VFS_BLOCKDEVICE = 0x04;

struct vnode_t;

/**
 * @brief Table of file operations function pointers
 */
struct vnode_operations_t {
    uint32_t (*read)(vnode_t* node, uint32_t offset, uint32_t size, uint8_t* buffer);
    uint32_t (*write)(vnode_t* node, uint32_t offset, uint32_t size, const uint8_t* buffer);
    void (*open)(vnode_t* node);
    void (*close)(vnode_t* node);
    vnode_t* (*finddir)(vnode_t* node, const char* name);
};

/**
 * @brief Universal Virtual File System Node Structure
 */
struct vnode_t {
    char name[128];
    uint32_t flags;
    uint32_t length;
    uint32_t inode;
    vnode_operations_t* ops;
    vnode_t* ptr;
};

/**
 * @brief Virtual File System Manager Class
 */
class VFS {
private:
    static vnode_t* m_root;

public:
    /**
     * @brief Initialize Virtual File System
     */
    static void init();

    /**
     * @brief Set root directory vnode
     */
    static void set_root(vnode_t* root);

    /**
     * @brief Get root directory vnode
     */
    static vnode_t* get_root();

    /**
     * @brief Read data from a VFS vnode
     */
    static uint32_t read(vnode_t* node, uint32_t offset, uint32_t size, uint8_t* buffer);

    /**
     * @brief Write data to a VFS vnode
     */
    static uint32_t write(vnode_t* node, uint32_t offset, uint32_t size, const uint8_t* buffer);

    /**
     * @brief Open a VFS vnode
     */
    static void open(vnode_t* node);

    /**
     * @brief Close a VFS vnode
     */
    static void close(vnode_t* node);

    /**
     * @brief Find a child file or directory by name inside a directory vnode
     */
    static vnode_t* finddir(vnode_t* node, const char* name);
};

} // namespace fs
} // namespace nebula

extern "C" {
    void vfs_init();
    uint32_t vfs_read(nebula::fs::vnode_t* node, uint32_t offset, uint32_t size, uint8_t* buffer);
    uint32_t vfs_write(nebula::fs::vnode_t* node, uint32_t offset, uint32_t size, const uint8_t* buffer);
}

#endif // NEBULA_FS_VFS_HPP
