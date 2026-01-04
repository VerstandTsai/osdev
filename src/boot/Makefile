BUILD = ../../build

.PHONY: all

all: $(BUILD)/bootsec $(BUILD)/boot

$(BUILD)/bootsec.elf: $(BUILD)/bootsec.o
	$(LD) -o $@ -Ttext 0x7c00 $(LDFLAGS) $^

$(BUILD)/boot.elf: $(BUILD)/boot.o $(BUILD)/fs.a
	$(LD) -o $@ -Ttext 0x10000 $(LDFLAGS) $^

$(BUILD)/bootsec.o: bootsec.s
	$(AS) -o $@ --32 $^

$(BUILD)/boot.o: boot.c
	$(CC) -o $@ $(CFLAGS) $^

$(BUILD)/fs.a: ../fs
	$(MAKE) -C $^

%: %.elf
	objcopy -O binary -j .text -j .data -j .bss $^ $@

