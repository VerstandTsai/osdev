#ifndef _TTY_H
#define _TTY_H

struct RingBuffer {
    char buffer[256];
    unsigned char head;
    unsigned char tail;
};

void ring_init(struct RingBuffer *ring);
void ring_push(struct RingBuffer *ring, char value);
char ring_pop(struct RingBuffer *ring);
int ring_empty(struct RingBuffer *ring);

void console_init();
void console_write(struct RingBuffer *);

void tty_init();
void tty_sendkey(int channel, char c);
void tty_write(int channel, const char *src, int count);

#endif // _TTY_H

