#ifndef _DEBUG_H
#define _DEBUG_H

#include "vesa.h"

#define COLOR(_x) { \
    for (int i=0; i<VIDEO_HEIGHT; i++) \
        for (int j=0; j<VIDEO_WIDTH; j++) \
            PUT_PIXEL(j + i * VIDEO_WIDTH, (_x)); \
}

#endif // _DEBUG_H

