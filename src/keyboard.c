#include <io.h>
#include <kernel.h>

#define CTRL 1
#define SHIFT 2
#define ALT 4

static int keystates = 0;

static char keymap_normal[] =
    "\0\x1b"                // empty, ESC
    "1234567890-=\x7f"      // ..., backspace
    "\tqwertyuiop[]\n"      // tab, ..., enter
    "\0asdfghjkl;'`\0\\"    // lctrl, ..., lshift, backslash
    "zxcvbnm,./\0"          // ..., rshift
    "*"                     // * (keypad)
    "\0 \0"                 // lalt, space, CapsLock
    "\0\0\0\0\0\0\0\0\0\0"  // F1-F10
    "\0\0"                  // NumLock, ScrollLock
    "789-"
    "456+"
    "123"
    "0."
    "\0\0\0"                // empty x3
    "\0\0"                  // F11, F12
;

static char keymap_shift[] =
    "\0\x1b"                // empty, ESC
    "!@#$%^&*()_+\x7f"      // ..., backspace
    "\tQWERTYUIOP{}\n"      // tab, ..., enter
    "\0ASDFGHJKL:\"~\0|"    // lctrl, ..., lshift, bar
    "ZXCVBNM<>?\0"          // ..., rshift
    "*"                     // * (keypad)
    "\0 \0"                 // lalt, space, CapsLock
    "\0\0\0\0\0\0\0\0\0\0"  // F1-F10
    "\0\0"                  // NumLock, ScrollLock
    "789-"
    "456+"
    "123"
    "0."
    "\0\0\0"                // empty x3
    "\0\0"                  // F11, F12
;

void keyboard_irq() {
    unsigned char scancode = inb(0x60);
    switch (scancode & 0x7f) {
    case 0x2a:
    case 0x36:
        if (scancode & 0x80)
            keystates ^= SHIFT;
        else
            keystates |= SHIFT;
        break;
    default:
        if (scancode & 0x80) break;
        char *keymap;
        switch (keystates) {
        case SHIFT:
            keymap = keymap_shift;
            break;
        default:
            keymap = keymap_normal;
        }
        printk("%c", keymap[scancode]);
    }
}

