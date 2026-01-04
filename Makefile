SRC 	= src
BUILD 	= build
BOOTSEC	= $(BUILD)/bootsec
BOOT	= $(BUILD)/boot
KERNEL  = $(BUILD)/kernel
DISKIMG = $(BUILD)/image.iso

CFLAGS  = -c -std=c99 -O3 -I../../include
CFLAGS += -Werror -Wall -Wextra
CFLAGS += -m32 -masm=intel -mno-sse
CFLAGS += -ffreestanding -fno-pie -fno-stack-protector

LDFLAGS = -m elf_i386 -L../../$(BUILD)

export CFLAGS
export LDFLAGS

.PHONY: all run clean

all: $(DISKIMG)

run: $(DISKIMG)
	qemu-system-i386 -drive format=raw,file=$(DISKIMG)

$(DISKIMG): $(BOOTSEC) $(BOOT)
	dd if=/dev/zero of=$@ bs=1G count=1
	dd if=$(BOOTSEC) of=$@ conv=notrunc bs=512 seek=0 count=1
	dd if=$(BOOT) of=$@ conv=notrunc bs=512 seek=1 count=15

$(BOOTSEC) $(BOOT): $(SRC)/boot
	$(MAKE) -C $^

clean:
	$(RM) $(BUILD)/*

