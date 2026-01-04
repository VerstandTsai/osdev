#include "disk.h"
#include "fs.h"
#include "debug.h"

void _start() {
    int color = 0x00ff00;
    int buffer;
    disk_write(130, &color, 1);
    disk_read(130, &buffer, 1);
    int inode = fs_inode_allocate();
    COLOR(inode);
    for (;;);
}

