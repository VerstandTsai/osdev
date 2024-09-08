#include <tty.h>
#include <termios.h>

void ring_init(struct RingBuffer *ring)             { ring->head = ring->tail = 0; }
void ring_push(struct RingBuffer *ring, char value) { ring->buffer[ring->tail++] = value; }
char ring_pop(struct RingBuffer *ring)              { return ring->buffer[ring->head++]; }
int ring_empty(struct RingBuffer *ring)             { return ring->head == ring->tail; }

struct Teletype {
    struct termios termios;
    struct RingBuffer input_queue;
    struct RingBuffer output_queue;
    void (*write)(struct RingBuffer*);
    int allocated;
};

void tty_init() {}

void tty_write(int channel, const char *src, int count) {
    console_write(src, count);
    channel = channel + 1; // TODO: remove this line
}

