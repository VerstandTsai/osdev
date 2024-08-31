#include <stdarg.h>
#include <kernel.h>
#include <tty.h>

static char buffer[1024];

int printk(const char *format, ...) {
    va_list ap;
    va_start(ap, format);
    int n = vsprintf(buffer, format, ap);
    va_end(ap);
    tty_write(0, buffer, n);
    return n;
}

