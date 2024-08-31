#include <tty.h>

void tty_init() {}

void tty_write(int channel, const char *src, int count) {
    console_write(src, count);
    channel = channel + 1; // TODO: remove this line
}

