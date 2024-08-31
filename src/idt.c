#include <idt.h>
#include <stdint.h>

#define IDT_NUM_ENTRIES 256

struct IDTEntry {
    uint16_t isr_low;
    uint16_t segment;
    uint8_t reserved;
    uint8_t attributes;
    uint16_t isr_high;
} __attribute__((packed));

struct IDTRegister {
    uint16_t size;
    uint32_t address;
} __attribute__((packed));

static struct IDTEntry idt[IDT_NUM_ENTRIES];
extern void *isr_stubs[IDT_NUM_ENTRIES];
void *isr_table[IDT_NUM_ENTRIES];

static void dummy() {};

void idt_set_gate(
    uint8_t vector,
    enum GateType type,
    enum PrivilegeLevel dpl,
    void (*isr)()
) {
    isr_table[vector] = isr;
    idt[vector].attributes |= 0x80; // Set the present bit
    idt[vector].attributes |= dpl << 5;
    idt[vector].attributes |= type;
    idt[vector].segment = 8; // Select the first entry in GDT
}

void idt_init() {
    for (int i=0; i<IDT_NUM_ENTRIES; i++) {
        idt_set_gate(i, INT_GATE_32, RING_0, dummy);
        idt[i].isr_high = (uint32_t)isr_stubs[i] >> 16;
        idt[i].isr_low = (uint32_t)isr_stubs[i];
    }
    struct IDTRegister idtr;
    idtr.address = (uint32_t)idt;
    idtr.size = sizeof(idt) - 1;
    __asm__("lidt %0" :: "m"(idtr));
}

