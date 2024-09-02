#include <kernel.h>
#include <tty.h>
#include <idt.h>

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
    //idt_set_gate(32, INT_GATE_32, RING_3, timer_irq);
    idt_set_gate(33, INT_GATE_32, RING_3, keyboard_irq);
    __asm__("sti");
}

