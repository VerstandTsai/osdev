#ifndef _IO_H
#define _IO_H

#define inb(port) ({ \
    int _x; \
    __asm__("in al, dx" : "=a"(_x) : "d"(port)); \
    _x; \
})

#define outb(value, port) \
    __asm__("out dx, al" :: "a"(value), "d"(port))

#endif // _IO_H

