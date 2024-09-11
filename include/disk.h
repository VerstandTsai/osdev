#ifndef _DISK_H
#define _DISK_H

#include <stdint.h>

void disk_irq();
void disk_read(uint64_t lba, void *dest, uint16_t count);
void disk_write(uint64_t lba, const void *src, uint16_t count);

#endif // _DISK_H

