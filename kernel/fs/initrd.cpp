/**
 * @file initrd.cpp
 * @brief Initrd RAM Disk driver implementation for Nebula OS
 * @author Nebula OS Team
 */

#include "../../include/kernel/fs/initrd.hpp"
#include "../../include/kernel/drivers/serial.hpp"

namespace nebula {
namespace fs {

vnode_t Initrd::m_initrd_dev;
vnode_t Initrd::m_initrd_root;
vnode_t Initrd::m_file_nodes[MAX_INITRD_FILES];
initrd_file_header_t Initrd::m_file_headers[MAX_INITRD_FILES];
size_t Initrd::m_file_count = 0;

vnode_operations_t Initrd::m_initrd_ops = {
    &Initrd::read,
    nullptr,
    nullptr,
    nullptr,
    &Initrd::finddir
};

static void str_copy(char* dest, const char* src, size_t max_len) {
    if (!dest || !src) return;
    size_t i = 0;
    while (src[i] != '\0' && i < max_len - 1) {
        dest[i] = src[i];
        i++;
    }
    dest[i] = '\0';
}

static bool str_equals(const char* s1, const char* s2) {
    if (!s1 || !s2) return false;
    size_t i = 0;
    while (s1[i] != '\0' && s2[i] != '\0') {
        if (s1[i] != s2[i]) return false;
        i++;
    }
    return s1[i] == s2[i];
}

// Memory-backed static initrd file contents for Nebula OS
static const char sample_file1_content[] = "Welcome to Nebula OS Filesystem! - Initrd RAM Disk test file.\n";
static const char sample_file2_content[] = "Nebula OS Userland Config: shell_theme=blue; autorun=true;\n";

vnode_t* Initrd::init(uintptr_t location) {
    (void)location;
    m_file_count = 2;

    // Root directory node
    str_copy(m_initrd_root.name, "initrd", sizeof(m_initrd_root.name));
    m_initrd_root.flags = VFS_DIRECTORY;
    m_initrd_root.length = 0;
    m_initrd_root.inode = 0;
    m_initrd_root.ops = &m_initrd_ops;
    m_initrd_root.ptr = nullptr;

    // File 1: hello.txt
    str_copy(m_file_nodes[0].name, "hello.txt", sizeof(m_file_nodes[0].name));
    m_file_nodes[0].flags = VFS_FILE;
    m_file_nodes[0].length = sizeof(sample_file1_content) - 1;
    m_file_nodes[0].inode = 1;
    m_file_nodes[0].ops = &m_initrd_ops;
    m_file_nodes[0].ptr = reinterpret_cast<vnode_t*>(const_cast<char*>(sample_file1_content));

    // File 2: config.sys
    str_copy(m_file_nodes[1].name, "config.sys", sizeof(m_file_nodes[1].name));
    m_file_nodes[1].flags = VFS_FILE;
    m_file_nodes[1].length = sizeof(sample_file2_content) - 1;
    m_file_nodes[1].inode = 2;
    m_file_nodes[1].ops = &m_initrd_ops;
    m_file_nodes[1].ptr = reinterpret_cast<vnode_t*>(const_cast<char*>(sample_file2_content));

    VFS::set_root(&m_initrd_root);
    nebula::drivers::Serial::write_string("[INITRD] Mounted RAM Disk at /initrd/ (Files: 2)\n");
    return &m_initrd_root;
}

uint32_t Initrd::read(vnode_t* node, uint32_t offset, uint32_t size, uint8_t* buffer) {
    if (!node || !buffer || (node->flags & VFS_DIRECTORY)) return 0;

    const char* content = reinterpret_cast<const char*>(node->ptr);
    if (!content || offset >= node->length) return 0;

    uint32_t bytes_to_read = size;
    if (offset + bytes_to_read > node->length) {
        bytes_to_read = node->length - offset;
    }

    for (uint32_t i = 0; i < bytes_to_read; i++) {
        buffer[i] = static_cast<uint8_t>(content[offset + i]);
    }

    return bytes_to_read;
}

vnode_t* Initrd::finddir(vnode_t* node, const char* name) {
    if (!node || !(node->flags & VFS_DIRECTORY) || !name) return nullptr;

    for (size_t i = 0; i < m_file_count; i++) {
        if (str_equals(m_file_nodes[i].name, name)) {
            return &m_file_nodes[i];
        }
    }
    return nullptr;
}

} // namespace fs
} // namespace nebula

extern "C" {
    nebula::fs::vnode_t* initrd_init(uintptr_t location) {
        return nebula::fs::Initrd::init(location);
    }
}
