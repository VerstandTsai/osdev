    .intel_syntax noprefix

    .altmacro

    .macro isr number, error
isr_\number:
    pusha
    call [isr_table+4*\number]
    popa

    .if \error
    add esp, 4
    .endif

    .if \number >= 32
    mov al, 0x20
    .if \number >= 32 + 8
    out 0xa0, al
    .endif
    out 0x20, al
    .endif

    iret
    .endm

    .macro isr_label number
    .long isr_\number
    .endm

    .text

    isr  0, 0
    isr  1, 0
    isr  2, 0
    isr  3, 0
    isr  4, 0
    isr  5, 0
    isr  6, 0
    isr  7, 0
    isr  8, 1
    isr  9, 0
    isr 10, 1
    isr 11, 1
    isr 12, 1
    isr 13, 1
    isr 14, 1
    isr 15, 0
    isr 16, 0
    isr 17, 1
    isr 18, 0
    isr 19, 0
    isr 20, 0
    isr 21, 0
    isr 22, 0
    isr 23, 0
    isr 24, 0
    isr 25, 0
    isr 26, 0
    isr 27, 0
    isr 28, 0
    isr 29, 1
    isr 30, 1
    isr 31, 0

    .set i, 32
    .rept 256-32
    isr %i, 0
    .set i, i+1
    .endr

    .data
    .global isr_stubs
isr_stubs:
    .set i, 0
    .rept 256
    isr_label %i
    .set i, i+1
    .endr

