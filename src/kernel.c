#include <kernel.h>
#include <tty.h>
#include <disk.h>
#include <idt.h>
#include <io.h>

#define printreg(reg) \
    __asm__( \
        "push " #reg "\n" \
        "push %0\n" \
        "call printk\n" \
        "add esp, 8\n" \
        :: "p"(#reg "=%#x\r\n") \
    )

void timer_irq() {
    printk("timer\r\n");
}

void kmain() {
    console_init();
    tty_init();
    idt_init();
    //idt_set_gate(32, INT_GATE_32, RING_0, timer_irq);
    idt_set_gate(33, INT_GATE_32, RING_0, keyboard_irq);
    idt_set_gate(46, INT_GATE_32, RING_0, disk_irq);
    __asm__("sti");
    unsigned char buffer[2048];
    disk_read(0, buffer, 2);
    printk("Before:\n");
    for (int i=0; i<1024; i++) {
        printk("%02x", buffer[i]);
    }
    disk_read(2, buffer, 2);
    printk("Setup:\n");
    for (int i=0; i<1024; i++) {
        printk("%02x", buffer[i]);
    }
    disk_write(0, buffer, 2);
    disk_read(0, buffer, 4);
    printk("After:\n");
    for (int i=0; i<2048; i++) {
        printk("%02x", buffer[i]);
    }
}

