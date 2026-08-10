/**
 * @file vfs.cpp
 * @brief Virtual File System (VFS) implementation for Nebula OS
 * @author Nebula OS Team
 */

#include <bsd/vfs/vfs.hpp>
#include <iokit/serial/serial.hpp>

namespace nebula {
namespace fs {

vnode_t* VFS::m_root = nullptr;

void VFS::init() {
    m_root = nullptr;
    nebula::drivers::Serial::write_string("[VFS] Virtual File System Initialized\n");
}

void VFS::set_root(vnode_t* root) {
    m_root = root;
}

vnode_t* VFS::get_root() {
    return m_root;
}

uint32_t VFS::read(vnode_t* node, uint32_t offset, uint32_t size, uint8_t* buffer) {
    if (node && node->ops && node->ops->read) {
        return node->ops->read(node, offset, size, buffer);
    }
    return 0;
}

uint32_t VFS::write(vnode_t* node, uint32_t offset, uint32_t size, const uint8_t* buffer) {
    if (node && node->ops && node->ops->write) {
        return node->ops->write(node, offset, size, buffer);
    }
    return 0;
}

void VFS::open(vnode_t* node) {
    if (node && node->ops && node->ops->open) {
        node->ops->open(node);
    }
}

void VFS::close(vnode_t* node) {
    if (node && node->ops && node->ops->close) {
        node->ops->close(node);
    }
}

vnode_t* VFS::finddir(vnode_t* node, const char* name) {
    if (node && (node->flags & VFS_DIRECTORY) && node->ops && node->ops->finddir) {
        return node->ops->finddir(node, name);
    }
    return nullptr;
}

} // namespace fs
} // namespace nebula

extern "C" {
    void vfs_init() {
        nebula::fs::VFS::init();
    }

    uint32_t vfs_read(nebula::fs::vnode_t* node, uint32_t offset, uint32_t size, uint8_t* buffer) {
        return nebula::fs::VFS::read(node, offset, size, buffer);
    }

    uint32_t vfs_write(nebula::fs::vnode_t* node, uint32_t offset, uint32_t size, const uint8_t* buffer) {
        return nebula::fs::VFS::write(node, offset, size, buffer);
    }
}
