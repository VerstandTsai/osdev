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

#define DISK_DRQ (1 << 3)
#define DISK_BSY (1 << 7)

static int reading;

static void disk_address(uint64_t lba, uint16_t sectors) {
    uint8_t *bytes = (uint8_t*)&lba;
    outb(DISK_DRIVE, 0x40);
    outb(DISK_SECTORS, sectors >> 8);
    outb(DISK_LBAL, bytes[3]);
    outb(DISK_LBAM, bytes[4]);
    outb(DISK_LBAH, bytes[5]);
    outb(DISK_SECTORS, sectors);
    outb(DISK_LBAL, bytes[0]);
    outb(DISK_LBAM, bytes[1]);
    outb(DISK_LBAH, bytes[2]);
}

void disk_irq() {
    if (reading) inb(DISK_CMD);
}

void disk_read(uint64_t lba, void *dest, uint16_t count) {
    reading = 1;
    disk_address(lba, count);
    outb(DISK_CMD, DISK_READ);
    while (count--) {
        while (!(inb(DISK_CMD) & DISK_DRQ));
        insw(DISK_IO, dest, 256);
        inb(DISK_CMD);
    }
}

void disk_write(uint64_t lba, const void *src, uint16_t count) {
    reading = 0;
    disk_address(lba, count);
    outb(DISK_CMD, DISK_WRITE);
    while (count--) {
        while (!(inb(DISK_CMD) & DISK_DRQ));
        outsw(DISK_IO, src, 256);
        inb(DISK_CMD);
    }
    outb(DISK_CMD, 0xe7);
    while (inb(DISK_CMD) & DISK_BSY);
}

