#ifndef _KERNEL_H
#define _KERNEL_H

#include <stdarg.h>

int printk(const char *format, ...);
int vsprintf(char *str, const char *format, va_list ap);

void keyboard_irq();

#endif // _KERNEL_H

