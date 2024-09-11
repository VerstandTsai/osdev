#ifndef _IO_H
#define _IO_H

#define inb(port) ({ \
    unsigned char _x; \
    __asm__("in al, dx" : "=a"(_x) : "d"(port)); \
    _x; \
})

#define insw(port, addr, count) \
    __asm__("cld; rep insw" :: "d"(port), "D"(addr), "c"(count))

#define outb(port, value) \
    __asm__("out dx, al" :: "d"(port), "a"(value))

#define outsw(port, addr, count) \
    __asm__("cld; rep outsw" :: "d"(port), "S"(addr), "c"(count))

#endif // _IO_H

