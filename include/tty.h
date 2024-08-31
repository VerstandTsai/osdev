#ifndef _TTY_H
#define _TTY_H

void console_init();
void console_write(const char *src, int count);
void tty_init();
void tty_write(int channel, const char *src, int count);

#endif // _TTY_H

