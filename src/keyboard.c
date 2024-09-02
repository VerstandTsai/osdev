#include <io.h>
#include <kernel.h>

void keyboard_irq() {
    char c = inb(0x60);
    printk("Key pressed: %#.2hhx\r\n", c);
}

