#include <tty.h>
#include <termios.h>
#include <string.h>

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

static struct Teletype ttys[64];

void tty_init() {
    ttys[0] = (struct Teletype){
        {
            ONLCR,
            ICANON | ECHO,
            SPECIAL_CHARS
        },
        {{}, 0, 0},
        {{}, 0, 0},
        console_write,
        1
    };
}

void tty_sendkey(int channel, char c) {
    struct Teletype *tty = ttys + channel;
    ring_push(&(tty->input_queue), c);
    if (tty->termios.c_lflag & ECHO && c >= 0) {
        char buffer[4];
        if (c < 32) {
            buffer[0] = '^';
            buffer[1] = c + '@';
            buffer[2] = '\0';
        } else {
            buffer[0] = c;
            buffer[1] = '\0';
        }
        tty_write(channel, buffer, strlen(buffer));
    }
}

void tty_write(int channel, const char *src, int count) {
    struct Teletype *tty = ttys + channel;
    for (int i=0; i<count; i++) {
        char c = src[i];
        if (tty->termios.c_oflag & ONLCR && c == '\n') {
            ring_push(&(tty->output_queue), '\r');
        }
        ring_push(&(tty->output_queue), c);
    }
    console_write(&(tty->output_queue));
}

