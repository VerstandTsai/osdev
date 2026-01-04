BUILD = ../../build

.PHONY: all

all: $(BUILD)/fs.a

$(BUILD)/fs.a: $(patsubst %.c,$(BUILD)/%.o,$(wildcard *.c))
	$(AR) r $@ $^

$(BUILD)/%.o: %.c
	$(CC) -o $@ $(CFLAGS) $^

