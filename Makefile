INCLUDE = include
SRC 	= src
BUILD 	= build
BOOT	= $(BUILD)/boot
KERNEL  = $(BUILD)/kernel
DISKIMG = $(BUILD)/image.iso

CFLAGS  = -I$(INCLUDE)
CFLAGS += -ffreestanding -fno-pie -fno-stack-protector
CFLAGS += -m32 -masm=intel -mno-sse
CFLAGS += -std=c99 -Werror -Wall -Wextra
CFLAGS += -O3

LDFLAGS = -m elf_i386

OBJECTS  = $(BUILD)/start.o $(BUILD)/kernel.o
OBJECTS += $(BUILD)/idt.o $(BUILD)/isr.o $(BUILD)/keyboard.o
OBJECTS += $(BUILD)/disk.o
OBJECTS += $(BUILD)/tty.o $(BUILD)/console.o $(BUILD)/vga8x16.o
OBJECTS += $(BUILD)/string.o $(BUILD)/vsprintf.o $(BUILD)/printk.o

.PHONY: all run clean

all: $(DISKIMG)

run: $(DISKIMG)
	qemu-system-i386 -drive format=raw,file=$(DISKIMG)

$(DISKIMG): $(BOOT) $(KERNEL)
	dd if=/dev/zero of=$@ bs=512 count=2048
	dd if=$(BOOT) of=$@ conv=notrunc bs=512 seek=0 count=1
	dd if=$(KERNEL) of=$@ conv=notrunc bs=512 seek=1 count=128

$(BOOT).elf:  $(BOOT).o
	$(LD) -o $@ $^ -Ttext 0x7c00 $(LDFLAGS)

$(KERNEL).elf: $(OBJECTS)
	$(LD) -o $@ $^ -Ttext 0x10000 $(LDFLAGS)

$(BUILD)/%.o: $(SRC)/%.c
	$(CC) -o $@ -c $^ $(CFLAGS)

$(BUILD)/%.o: $(SRC)/%.s
	$(AS) -o $@ $^ --32

$(BUILD)/%.o: $(SRC)/%.font
	$(LD) -o $@ -b binary $^ -r $(LDFLAGS)

$(BUILD)/%: $(BUILD)/%.elf
	objcopy -O binary $^ $@

clean:
	$(RM) $(BUILD)/*

