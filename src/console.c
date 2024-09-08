#include <stdint.h>
#include <string.h>
#include <vesa.h>
#include <tty.h>

#define FONT _binary_src_vga8x16_font_start
#define FONT_WIDTH 8
#define FONT_HEIGHT 16

#define SCROLL_UP_IF_NEEDED() { \
    if (cursor.y >= console_height) { \
        scroll_up(cursor.y - console_height + 1); \
        cursor.y = console_height - 1; \
    } \
}

#define TABSIZE 8
#define HT() { advance_cursor(TABSIZE); cursor.x = cursor.x / 8 * 8; }
#define LF() { cursor.y++; SCROLL_UP_IF_NEEDED(); }
#define CR() { cursor.x = 0; }
#define DEL() { advance_cursor(-1); draw_character(' '); }

struct TextCursor {
    uint8_t shape[FONT_HEIGHT];
    int x;
    int y;
};

enum ConsoleState {
    STATE_NORMAL,
    STATE_ESC,
    STATE_CSI
};

enum VGAColor {
    VGA_BLACK,
    VGA_RED,
    VGA_GREEN,
    VGA_YELLOW,
    VGA_BLUE,
    VGA_MAGENTA,
    VGA_CYAN,
    VGA_WHITE,
    VGA_LBLACK,
    VGA_LRED,
    VGA_LGREEN,
    VGA_LYELLOW,
    VGA_LBLUE,
    VGA_LMAGENTA,
    VGA_LCYAN,
    VGA_LWHITE
};

const uint32_t palette[] = {
    0x000000,
    0xaa0000,
    0x00aa00,
    0xaaaa00,
    0x0000aa,
    0xaa00aa,
    0x00aaaa,
    0xaaaaaa,
    0x555555,
    0xff5555,
    0x55ff55,
    0xffff55,
    0x5555ff,
    0xff55ff,
    0x55ffff,
    0xffffff,
};

extern uint8_t FONT[];

static int console_width;
static int console_height;

static struct TextCursor cursor;

static enum VGAColor fg;
static enum VGAColor bg;

static enum ConsoleState state;

static void draw_cursor() {
    int index = (cursor.y * FONT_HEIGHT * VIDEO_WIDTH + cursor.x * FONT_WIDTH) * 3;
    for (int i=0; i<FONT_HEIGHT; i++) {
        for (int j=0; j<FONT_WIDTH; j++) {
            if ((cursor.shape[i] << j) & 0x80) {
                FRAMEBUFFER[index] = ~FRAMEBUFFER[index];
                FRAMEBUFFER[index+1] = ~FRAMEBUFFER[index+1];
                FRAMEBUFFER[index+2] = ~FRAMEBUFFER[index+2];
            }
            index += 3;
        }
        index += VIDEO_PITCH - FONT_WIDTH * 3;
    }
}

static void scroll_up(int lines) {
    int i = lines * FONT_HEIGHT * VIDEO_PITCH;
    int j = VIDEO_PITCH * VIDEO_HEIGHT - i;
    memmove(FRAMEBUFFER, FRAMEBUFFER + i, j);
    memset(FRAMEBUFFER + j, 0, i);
}

static void advance_cursor(int n) {
    int pos = cursor.y * console_width + cursor.x + n;
    cursor.y = pos / console_width;
    cursor.x = pos % console_width;
    SCROLL_UP_IF_NEEDED();
}

static void draw_character(char ch) {
    uint8_t *glyph = FONT + ch * FONT_HEIGHT;
    int pos = cursor.y * FONT_HEIGHT * VIDEO_WIDTH + cursor.x * FONT_WIDTH;
    uint32_t fg_color = palette[fg];
    uint32_t bg_color = palette[bg];
    for (int i=0; i<FONT_HEIGHT; i++) {
        for (int j=0; j<FONT_WIDTH; j++) {
            PUT_PIXEL(pos, (glyph[i] << j) & 0x80 ? fg_color : bg_color);
            pos++;
        }
        pos += VIDEO_WIDTH - FONT_WIDTH;
    }
}

static void console_putc(char ch) {
    switch (state) {
    case STATE_NORMAL:
        switch (ch) {
        case '\0': break;
        case '\t': HT(); break;
        case '\n': LF(); break;
        case '\r': CR(); break;
        case '\x7f': DEL(); break;
        default:
            draw_character(ch);
            advance_cursor(1);
        }
        break;
    default:;
    }
}

void console_init() {
    cursor = (struct TextCursor) {
        {
            0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00,
            0xff, 0xff, 0xff, 0xff,
        }, 0, 0
    };
    console_width = VIDEO_WIDTH / FONT_WIDTH;
    console_height = VIDEO_HEIGHT / FONT_HEIGHT;
    fg = VGA_WHITE;
    bg = VGA_BLACK;
    state = STATE_NORMAL;
    draw_cursor();
}

void console_write(struct RingBuffer *ring) {
    draw_cursor(); // Actually un-draws the cursor
    while (!ring_empty(ring)) {
        console_putc(ring_pop(ring));
    }
    draw_cursor();
}

