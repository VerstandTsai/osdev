CC = gcc
AS = as
LD = ld

CFLAGS  = -I./include
CFLAGS += -fno-pie -ffreestanding -masm=intel -m32
CFLAGS += -std=c99 -Werror -Wall -Wextra
CFLAGS += -O3

OBJECTS  = build/start.o build/kernel.o
OBJECTS += build/idt.o build/isr.o build/keyboard.o
OBJECTS += build/tty.o build/console.o build/vga8x16.o
OBJECTS += build/string.o build/vsprintf.o build/printk.o

DISKIMG = disk.iso

.PHONY: all run clean

all: $(DISKIMG)

run: $(DISKIMG)
	qemu-system-x86_64 -drive format=raw,file=disk.iso

$(DISKIMG): bin/boot bin/kernel
	dd if=/dev/zero of=$@ bs=512 count=2048
	dd if=bin/boot of=$@ conv=notrunc bs=512 seek=0 count=1
	dd if=bin/kernel of=$@ conv=notrunc bs=512 seek=1 count=128

bin/boot: build/boot.o
	$(LD) -o $@ $^ -Ttext 0x7c00 --oformat binary

build/boot.o: src/boot.s
	$(AS) -o $@ $<

bin/kernel: $(OBJECTS)
	$(LD) -o $@ $^ -Ttext 0x10000 --oformat binary -m elf_i386

build/%.o: src/%.c
	$(CC) -o $@ -c $< $(CFLAGS)

build/%.o: src/%.s
	$(AS) -o $@ $< --32

build/%.o: src/%.font
	$(LD) -o $@ -b binary $< -r -m elf_i386

clean:
	$(RM) build/* bin/*

