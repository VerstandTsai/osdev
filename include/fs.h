#ifndef _FS_H
#define _FS_H

#include <stdint.h>

struct Inode {
    int64_t timestamp;
    uint64_t size;
    uint32_t type;
    uint32_t permisson;
    uint32_t uid;
    uint32_t gid;
    uint32_t link_count;
    uint32_t direct[4];
    uint32_t indirect1;
    uint32_t indirect2;
    uint32_t indirect3;
};

uint32_t fs_inode_allocate();
void fs_inode_free(uint32_t inode);
void fs_inode_get(uint32_t inode, struct Inode *buffer);
void fs_inode_set(uint32_t inode, const struct Inode *buffer);
void fs_data_read(uint32_t inode, int offset, void *buffer, int len);
void fs_data_write(uint32_t inode, int offset, const void *buffer, int len);

#endif // _FS_H

