#ifndef _INTERRUPT_H
#define _INTERRUPT_H

#include <stdint.h>

enum PrivilegeLevel {
    RING_0,
    RING_1,
    RING_2,
    RING_3
};

enum GateType {
    TASK_GATE = 5,
    INT_GATE_16 = 6,
    TRAP_GATE_16 = 7,
    INT_GATE_32 = 14,
    TRAP_GATE_32 = 15
};

struct SavedRegs {
    uint32_t edi;
    uint32_t esi;
    uint32_t ebp;
    uint32_t esp;
    uint32_t ebx;
    uint32_t edx;
    uint32_t ecx;
    uint32_t eax;
};

void idt_init();
void idt_set_gate(
    uint8_t vector,
    enum GateType,
    enum PrivilegeLevel,
    void (*isr)()
);

#endif // _INTERRUPT_H

