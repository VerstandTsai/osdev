#ifndef _IO_H
#define _IO_H

#define inb(port) ({ \
    unsigned char _x; \
    __asm__("in al, dx" : "=a"(_x) : "d"(port)); \
    _x; \
})

#define outb(port, value) \
    __asm__("out dx, al" :: "d"(port), "a"(value))

#endif // _IO_H

