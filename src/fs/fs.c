#include "fs.h"
#include "disk.h"
#include <stdint.h>

#define BYTES_PER_SECTOR 512
#define BYTES_PER_BLOCK 4096
#define SECTORS_PER_BLOCK (BYTES_PER_BLOCK / BYTES_PER_SECTOR)
#define SUPER_SECTOR 16
#define GROUPS 8192
#define INODES_PER_SECTOR (BYTES_PER_SECTOR / sizeof(struct Inode))
#define INODES_PER_GROUP 32768
#define DATA_PER_GROUP 32768

#define GROUP_SECTOR(_i) (SUPER_SECTOR + SECTORS_PER_BLOCK + (uint64_t)(_i) * sizeof(struct BlockGroup) / BYTES_PER_SECTOR)

#define BITMAP_TEST(_bitmap, _i) ((_bitmap)[(_i) >> 5] & (1 << ((_i) & 0x1f)))
#define BITMAP_SET(_bitmap, _i) ((_bitmap)[(_i) >> 5] |= (1 << ((_i) & 0x1f)))
#define BITMAP_RESET(_bitmap, _i) ((_bitmap)[(_i) >> 5] &= ~(1 << ((_i) & 0x1f)))

struct Super {
    uint32_t good_groups[GROUPS >> 5];
    uint32_t inode_full[GROUPS >> 5];
    uint32_t data_full[GROUPS >> 5];
    uint32_t reserved[GROUPS >> 5];
};

struct BlockGroup {
    uint32_t inode_bitmap[INODES_PER_GROUP >> 5];
    uint32_t data_bitmap[DATA_PER_GROUP >> 5];
    struct Inode inodes[INODES_PER_GROUP];
    uint8_t data[DATA_PER_GROUP][BYTES_PER_BLOCK];
};

uint64_t locate_inode(uint32_t inode) {
    int group = inode / INODES_PER_GROUP;
    int index = inode % INODES_PER_GROUP;
    uint64_t sector = GROUP_SECTOR(group) + 2 * SECTORS_PER_BLOCK + index / INODES_PER_SECTOR;
    return (sector << 16) | (index % INODES_PER_SECTOR);
}

int allocate_group() {
    struct Super super;
    disk_read(SUPER_SECTOR, &super, SECTORS_PER_BLOCK);
    int group;
    for (group=0; BITMAP_TEST(super.good_groups, group); group++);

    BITMAP_SET(super.good_groups, group);
    BITMAP_RESET(super.inode_full, group);
    BITMAP_RESET(super.data_full, group);
    disk_write(SUPER_SECTOR, &super, SECTORS_PER_BLOCK);

    uint8_t zeros[2 * BYTES_PER_BLOCK] = {0};
    disk_write(GROUP_SECTOR(group), &zeros, 2 * SECTORS_PER_BLOCK);
    return group;
}

void read_data(uint32_t pointer, void *buffer) {
    int group = pointer / DATA_PER_GROUP;
    int index = pointer % DATA_PER_GROUP;
    uint64_t sector =
        GROUP_SECTOR(group) +
        2 * SECTORS_PER_BLOCK +
        INODES_PER_GROUP / INODES_PER_SECTOR +
        index * SECTORS_PER_BLOCK;
    disk_read(sector, buffer, SECTORS_PER_BLOCK);
}

void free_data(uint32_t pointer, int level) {
    if (!pointer) return;
    int group = pointer / DATA_PER_GROUP;
    int index = pointer % DATA_PER_GROUP;

    struct Super super;
    disk_read(SUPER_SECTOR, &super, SECTORS_PER_BLOCK);
    BITMAP_RESET(super.data_full, group);
    disk_write(SUPER_SECTOR, &super, SECTORS_PER_BLOCK);

    uint32_t data_bitmap[DATA_PER_GROUP >> 5];
    disk_read(GROUP_SECTOR(group) + SECTORS_PER_BLOCK, data_bitmap, SECTORS_PER_BLOCK);
    BITMAP_RESET(data_bitmap, index);
    disk_write(GROUP_SECTOR(group) + SECTORS_PER_BLOCK, data_bitmap, SECTORS_PER_BLOCK);

    if (!level) return;
    uint32_t buffer[BYTES_PER_BLOCK / sizeof(uint32_t)];
    read_data(pointer, buffer);
    for (unsigned int i=0; i<BYTES_PER_BLOCK / sizeof(uint32_t); i++)
        free_data(buffer[i], level - 1);
}

uint32_t fs_inode_allocate() {
    struct Super super;
    disk_read(SUPER_SECTOR, &super, SECTORS_PER_BLOCK);
    int group;
    for (
        group=0;
        group<GROUPS && (
            !BITMAP_TEST(super.good_groups, group) ||
            BITMAP_TEST(super.inode_full, group));
        group++
    );
    if (group >= GROUPS) group = allocate_group();

    uint32_t inode_bitmap[INODES_PER_GROUP >> 5];
    disk_read(GROUP_SECTOR(group), inode_bitmap, SECTORS_PER_BLOCK);
    int index;
    for (index=!group; BITMAP_TEST(inode_bitmap, index); index++);
    BITMAP_SET(inode_bitmap, index);
    disk_write(GROUP_SECTOR(group), inode_bitmap, SECTORS_PER_BLOCK);

    uint32_t mask = 0xffffffff;
    for (int i=0; i<INODES_PER_GROUP >> 5; i++) mask &= inode_bitmap[i];
    if (mask == 0xffffffff) {
        BITMAP_SET(super.inode_full, group);
        disk_write(SUPER_SECTOR, &super, SECTORS_PER_BLOCK);
    }

    uint32_t inode = index + group * INODES_PER_GROUP;
    uint64_t location = locate_inode(inode);
    uint64_t sector = location >> 16;
    struct Inode inodes[INODES_PER_SECTOR];
    disk_read(sector, inodes, 1);
    inodes[location % INODES_PER_SECTOR] = (struct Inode){0};
    disk_write(sector, inodes, 1);
    return inode;
}

void fs_inode_free(uint32_t inode) {
    int group = inode / INODES_PER_GROUP;
    int index = inode % INODES_PER_GROUP;

    struct Super super;
    disk_read(SUPER_SECTOR, &super, SECTORS_PER_BLOCK);
    BITMAP_RESET(super.inode_full, group);
    disk_write(SUPER_SECTOR, &super, SECTORS_PER_BLOCK);

    uint32_t inode_bitmap[INODES_PER_GROUP >> 5];
    disk_read(GROUP_SECTOR(group), inode_bitmap, SECTORS_PER_BLOCK);
    BITMAP_RESET(inode_bitmap, index);
    disk_write(GROUP_SECTOR(group), inode_bitmap, SECTORS_PER_BLOCK);

    struct Inode buffer;
    fs_inode_get(inode, &buffer);
    for (int i=0; i<4; i++)
        free_data(buffer.direct[i], 0);
    free_data(buffer.indirect1, 1);
    free_data(buffer.indirect2, 2);
    free_data(buffer.indirect3, 3);
}

void fs_inode_get(uint32_t inode, struct Inode *buffer) {
    uint64_t location = locate_inode(inode);
    uint64_t sector = location >> 16;
    struct Inode inodes[INODES_PER_SECTOR];
    disk_read(sector, inodes, 1);
    *buffer = inodes[location % INODES_PER_SECTOR];
}

void fs_inode_set(uint32_t inode, const struct Inode *buffer) {
    uint64_t location = locate_inode(inode);
    uint64_t sector = location >> 16;
    struct Inode inodes[INODES_PER_SECTOR];
    disk_read(sector, inodes, 1);
    struct Inode *p = inodes + location % INODES_PER_SECTOR;
    p->timestamp = buffer->timestamp;
    p->size = buffer->size;
    p->type = buffer->type;
    p->permisson = buffer->permisson;
    p->uid = buffer->uid;
    p->gid = buffer->gid;
    p->link_count = buffer->link_count;
    disk_write(sector, inodes, 1);
}

/*
void fs_data_read(uint32_t inode, int offset, void *buffer, int len);
    ;
}

void fs_data_write(uint32_t inode, int offset, const void *buffer, int len);
    ;
}
*/

