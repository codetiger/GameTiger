#include "pico/stdlib.h"

#ifndef _GAME_TIGER_COLOR_H
#define _GAME_TIGER_COLOR_H

typedef struct Color {
    uint16_t red:5;
    uint16_t green:6;
    uint16_t blue:5;

    Color() {
        red = 0; green = 0; blue = 0;
    }

    Color(uint8_t r, uint8_t g, uint8_t b) {
        red = r >> 3; green = g >> 2; blue = b >> 3;
    }
} Color;

#endif