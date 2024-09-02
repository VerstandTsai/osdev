#include <kernel.h>
#include <tty.h>
#include <interrupt.h>

#define printreg(reg) \
    __asm__( \
        "push " #reg "\n" \
        "push %0\n" \
        "call printk\n" \
        "add esp, 8\n" \
        :: "p"(#reg "=%#x\r\n") \
    )

void timer_interrupt() {
    printk("timer\r\n");
}

void keyboard_interrupt() {
    printk("keyboard\r\n");
}

void syscall(const struct SavedRegs regs) {
    printk("System call invoked with eax=%#x\r\n", regs.eax);
    printk("System call invoked with ebx=%#x\r\n", regs.ebx);
    printk("System call invoked with ecx=%#x\r\n", regs.ecx);
    printk("System call invoked with edx=%#x\r\n", regs.edx);
    printk("System call invoked with edi=%#x\r\n", regs.edi);
    printk("System call invoked with esi=%#x\r\n", regs.esi);
    printk("System call invoked with ebp=%#x\r\n", regs.ebp);
    printk("System call invoked with esp=%#x\r\n", regs.esp);
}

void kmain() {
    console_init();
    tty_init();
    idt_init();
    idt_set_gate(32, INT_GATE_32, RING_3, timer_interrupt);
    idt_set_gate(33, INT_GATE_32, RING_3, keyboard_interrupt);
    idt_set_gate(0x80, INT_GATE_32, RING_3, syscall);
    __asm__("sti");
    __asm__("mov eax, 0x9487");
    __asm__("mov ebx, 0x9");
    __asm__("mov ecx, 0x8");
    __asm__("mov edx, 0x7");
    __asm__("mov edi, 0x42");
    __asm__("mov esi, 0x69");
    __asm__("int 0x80");
    __asm__("int 0x80");
    __asm__("int 0x80");
}

