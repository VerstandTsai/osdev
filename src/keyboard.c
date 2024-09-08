#include <io.h>
#include <tty.h>
#include <kernel.h>

#define LCTRL 1
#define RCTRL 2
#define LALT 4
#define RALT 8
#define LSHIFT 16
#define RSHIFT 32

static int keystates = 0;

static int extend = 0;

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

static void press_key(unsigned char scancode) {
    switch (scancode) {
        char c;
    case 0x2a: keystates |= LSHIFT; break;
    case 0x36: keystates |= RSHIFT; break;
    case 0x1d: keystates |= LCTRL; break;
    default:
        if (keystates & (LCTRL | RCTRL)) {
            c = keymap_shift[scancode];
            if (c == '{' || c == '|' || c == '}')
                c = keymap_normal[scancode];
            c -= '@';
        } else {
            char *keymap;
            if (keystates & (LSHIFT | RSHIFT)) {
                keymap = keymap_shift;
            } else {
                keymap = keymap_normal;
            }
            c = keymap[scancode];
        }
        tty_sendkey(0, c);
    }
}

static void release_key(unsigned char scancode) {
    switch (scancode) {
    case 0x2a: keystates &= ~LSHIFT; break;
    case 0x36: keystates &= ~RSHIFT; break;
    case 0x1d: keystates &= ~LCTRL; break;
    default: break;
    }
}

void keyboard_irq() {
    unsigned char scancode = inb(0x60);
    if (scancode == 0xe0) {
        extend = 1;
    } else if (scancode & 0x80) {
        release_key(scancode & 0x7f);
    } else {
        press_key(scancode);
    }
}

