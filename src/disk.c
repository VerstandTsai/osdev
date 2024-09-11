#include <disk.h>
#include <stdint.h>
#include <io.h>

#define DISK_IO 0x1f0
#define DISK_SECTORS (DISK_IO + 2)
#define DISK_LBAL (DISK_IO + 3)
#define DISK_LBAM (DISK_IO + 4)
#define DISK_LBAH (DISK_IO + 5)
#define DISK_DRIVE (DISK_IO + 6)
#define DISK_CMD (DISK_IO + 7)

#define DISK_READ 0x24
#define DISK_WRITE 0x34

static int reading;
static uint16_t *ptr;

static void disk_address(uint64_t lba, uint16_t count) {
    uint8_t *bytes = (uint8_t*)&lba;
    outb(DISK_DRIVE, 0x40);
    outb(DISK_SECTORS, count >> 8);
    outb(DISK_LBAL, bytes[3]);
    outb(DISK_LBAM, bytes[4]);
    outb(DISK_LBAH, bytes[5]);
    outb(DISK_SECTORS, count);
    outb(DISK_LBAL, bytes[0]);
    outb(DISK_LBAM, bytes[1]);
    outb(DISK_LBAH, bytes[2]);
}

void disk_irq() {
    if (reading) insw(DISK_IO, ptr, 256);
    else outsw(DISK_IO, ptr, 256);
    ptr += 256;
}

void disk_read(uint64_t lba, void *dest, uint16_t count) {
    reading = 1;
    ptr = dest;
    disk_address(lba, count);
    outb(DISK_CMD, DISK_READ);
}

void disk_write(uint64_t lba, const void *src, uint16_t count) {
    reading = 0;
    ptr = (uint16_t*)src;
    disk_address(lba, count);
    outb(DISK_CMD, DISK_WRITE);
}

