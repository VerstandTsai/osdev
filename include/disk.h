#ifndef _DISK_H
#define _DISK_H

#include <stdint.h>

void disk_read(uint64_t lba, void *dest, int count);
void disk_write(uint64_t lba, const void *src, int count);

#endif // _DISK_H

