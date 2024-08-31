#ifndef _VESA_H
#define _VESA_H

#include <stdint.h>

struct VesaModeInfo {
    uint16_t attributes;    // deprecated, only bit 7 should be of interest to you, and it indicates the mode supports a linear frame buffer.
    uint8_t window_a;       // deprecated
    uint8_t window_b;       // deprecated
    uint16_t granularity;   // deprecated; used while calculating bank numbers
    uint16_t window_size;
    uint16_t segment_a;
    uint16_t segment_b;
    uint32_t win_func_ptr;  // deprecated; used to switch banks from protected mode without returning to real mode
    uint16_t pitch;         // number of bytes per horizontal line
    uint16_t width;         // width in pixels
    uint16_t height;        // height in pixels
    uint8_t w_char;         // unused...
    uint8_t y_char;         // ...
    uint8_t planes;
    uint8_t bpp;            // bits per pixel in this mode
    uint8_t banks;          // deprecated; total number of banks in this mode
    uint8_t memory_model;
    uint8_t bank_size;      // deprecated; size of a bank, almost always 64 KB but may be 16 KB...
    uint8_t image_pages;
    uint8_t reserved0;

    uint8_t red_mask;
    uint8_t red_position;
    uint8_t green_mask;
    uint8_t green_position;
    uint8_t blue_mask;
    uint8_t blue_position;
    uint8_t reserved_mask;
    uint8_t reserved_position;
    uint8_t direct_color_attributes;

    uint32_t framebuffer;           // physical address of the linear frame buffer; write here to draw to the screen
    uint32_t off_screen_mem_off;
    uint16_t off_screen_mem_size;   // size of memory in the framebuffer but not being displayed on the screen
    uint8_t reserved1[206];
} __attribute__((packed));

#define VIDEO_MODE_INFO ((struct VesaModeInfo *)0x1000)
#define VIDEO_PITCH (VIDEO_MODE_INFO->pitch)
#define VIDEO_WIDTH (VIDEO_MODE_INFO->width)
#define VIDEO_HEIGHT (VIDEO_MODE_INFO->height)
#define FRAMEBUFFER ((uint8_t*)(VIDEO_MODE_INFO->framebuffer))
#define PUT_PIXEL(pos, color) { \
    int _i = (pos) * 3; \
    uint32_t _c = (color); \
    FRAMEBUFFER[_i] = _c & 0xff; \
    FRAMEBUFFER[_i+1] = (_c >> 8) & 0xff; \
    FRAMEBUFFER[_i+2] = _c >> 16; \
}

#endif // _VESA_H

