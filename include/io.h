#ifndef _IO_H
#define _IO_H

#define inb(port) ({ \
    unsigned char _x; \
    __asm__ volatile ("in al, dx" : "=a"(_x) : "d"(port)); \
    _x; \
})

#define insw(port, addr, count) \
    __asm__ volatile ("cld; rep insw" : "+D"(addr) : "d"(port), "c"(count))

#define outb(port, value) \
    __asm__ volatile ("out dx, al" :: "d"(port), "a"(value))

#define outsw(port, addr, count) \
    __asm__ volatile ("cld; rep outsw" : "+S"(addr) : "d"(port), "c"(count))

#endif // _IO_H

