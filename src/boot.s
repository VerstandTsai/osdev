    .intel_syntax noprefix

    .text
    .global _start

    .code16
_start:
    # Initialize registers
    mov ax, cs
    mov ds, ax
    mov es, ax
    mov ss, ax
    mov sp, 0x9000

    # Load the kernel at 0x10000
    mov ax, 0x1000
    mov es, ax
    xor bx, bx
    mov ah, 0x02
    mov al, 128         # Number of sectors to read
    xor dh, dh          # Head 0
    xor ch, ch          # Cylinder 0
    mov cl, 2           # Sector 2
    int 0x13

    # Setup Vesa video mode
    xor ax, ax
    mov es, ax
    mov di, 0x1000      # Store Vesa video mode info at 0x1000
    mov ax, 0x4f01      # Get Vesa video mode info
    mov cx, 0x4118      # Mode 118h (1024x768x24)
    int 0x10

    mov ax, 0x4f02      # Set Vesa video mode
    mov bx, cx
    int 0x10

    # Enter protected mode
    cli
    lgdt [gdtr]
    mov eax, cr0
    or eax, 1
    mov cr0, eax
    jmp (gdt_code-gdt):enter32

    .code32
enter32:
    # Setup segment registers
    mov ax, gdt_data-gdt
    mov ds, ax
    mov es, ax
    mov ss, ax

    # Setup the stack
    mov esp, 0xe00000
    mov ebp, esp

    # Jump to kernel
    jmp 0x10000

gdt:
    .word 0, 0, 0, 0    # Null descriptor

gdt_code:
    .word 0xffff        # Limit[15:0]
    .word 0             # Base [15:0]
    .byte 0             # Base [23:16]
    .byte 0b10011010    # Code (read/executable)
    .byte 0b11001111    # 4KiB blocks, Limit[19:16]
    .byte 0             # Base [31:24]

gdt_data:
    .word 0xffff        # Limit[15:0]
    .word 0             # Base [15:0]
    .byte 0             # Base [23:16]
    .byte 0b10010010    # Data (read/writable)
    .byte 0b11001111    # 4KiB blocks, Limit[19:16]
    .byte 0             # Base [31:24]

gdt_end:

gdtr:
    .word gdt_end - gdt - 1
    .long gdt

    .org 510
    .word 0xaa55

