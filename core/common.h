#ifdef FORMPU
    #include "pico/stdlib.h"
    #include "pico/multicore.h"
    #include "hardware/gpio.h"
    #include "hardware/adc.h"
    #include "hardware/pwm.h"
    #include "hardware/spi.h"
    #include "hardware/dma.h"
    #include "hardware/flash.h"
    #include "hardware/sync.h"
    #include "hardware/clocks.h"

    #define timetype uint32_t
#else
    #include <cstdint>
    #include <SDL.h>
    #include <chrono>

    typedef std::chrono::high_resolution_clock Clock;
    typedef std::chrono::milliseconds milliseconds;
    #define timetype Clock::time_point
    #define FLASH_PAGE_SIZE (1u << 8)
#endif

#include <map>
#include <string>
#include <cstring>
#include <array>
#include <math.h>
#include <algorithm>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <queue>

#ifndef _GAME_TIGER_COMMON_H
#define _GAME_TIGER_COMMON_H

typedef struct Vec2 {
    int16_t x, y;

    Vec2() {
        x = y = 0;
    }

    Vec2(int16_t _x, int16_t _y) {
        x = _x; y = _y;
    }

    Vec2 operator+(const uint16_t &v) {
        return Vec2(this->x + v, this->y + v);
    };

    Vec2 operator-(const Vec2 &v) {
        return Vec2(this->x - v.x, this->y - v.y);
    };

    Vec2 operator+(const Vec2 &v) {
        return Vec2(this->x + v.x, this->y + v.y);
    };

    Vec2 operator*(const float &v) {
        return Vec2(this->x * v, this->y * v);
    };

    // Vec2 operator*(const uint16_t &v) {
    //     return Vec2(this->x * v, this->y * v);
    // };
} Vec2;

typedef struct Size2 {
    uint16_t w, h;

    Size2() {
        w = h = 0;
    }

    Size2(uint16_t _w, uint16_t _h) {
        w = _w; h = _h;
    }

    Size2 operator*(const uint16_t &v) {
        Size2 result;
        result.w = this->w * v;
        result.h = this->h * v;
        return result;
    };
} Size2;

typedef struct Rect2 {
    int16_t x, y;
    uint16_t w, h;

    Rect2() {
        x = y = w = h = 0;
    }

    Rect2(Vec2 _p, Size2 _s) {
        x = _p.x; y = _p.y;
        w = _s.w; h = _s.h;
    }

    Rect2(int16_t _x, int16_t _y, uint16_t _w, uint16_t _h) {
        x = _x; y = _y;
        w = _w; h = _h;
    }
} Rect2;

#include "color.h"
#include "../content/allGameSprite.h"
#include "../content/allGameAlphaSprite.h"
#include "../content/menuSprite.h"
#include "../content/alphanumfont.h"

#include "../core/image.h"
#include "tilemap.h"
#include "level.h"

#define DISPLAY_WIDTH 320
#define DISPLAY_HEIGHT 240

#define AUDIO_FLAG_VALUE 42

enum ImageType {INDEXEDCOLORS, PIXELCOLOR};
enum GameState {WAITING, PLAYING, PAUSED, LOST};

timetype getTime();
uint16_t getTimeDiffMS(timetype start);

const int8_t sinlut[180] = {0, 3, 6, 10, 13, 17, 20, 24, 27, 30, 34, 37, 40, 43, 46, 49, 52, 55, 
                            58, 61, 64, 66, 69, 71, 74, 76, 78, 80, 82, 84, 86, 88, 89, 91, 92, 93,
                            95, 96, 97, 97, 98, 99, 99, 99, 99, 100, 99, 99, 99, 99, 98, 97, 97, 96,
                            95, 93, 92, 91, 89, 88, 86, 84, 82, 80, 78, 76, 74, 71, 69, 66, 64, 61,
                            58, 55, 52, 49, 46, 43, 40, 37, 34, 30, 27, 24, 20, 17, 13, 10, 6, 3,
                            0, -3, -6, -10, -13, -17, -20, -24, -27, -30, -34, -37, -40, -43, -46, -50, -52, -55,
                            -58, -61, -64, -66, -69, -71, -74, -76, -78, -80, -82, -84, -86, -88, -89, -91, -92, -93,
                            -95, -96, -97, -97, -98, -99, -99, -99, -99, -100, -99, -99, -99, -99, -98, -97, -97, -96,
                            -95, -93, -92, -91, -89, -88, -86, -84, -82, -80, -78, -76, -74, -71, -69, -66, -64, -61,
                            -58, -55, -52, -50, -46, -43, -40, -37, -34, -30, -27, -24, -20, -17, -13, -10, -6, -3};

const int8_t coslut[180] = {100, 99, 99, 99, 99, 98, 97, 97, 96, 95, 93, 92, 91, 89, 88, 86, 84, 82,
                            80, 78, 76, 74, 71, 69, 66, 64, 61, 58, 55, 52, 50, 46, 43, 40, 37, 34,
                            30, 27, 24, 20, 17, 13, 10, 6, 3, 0, -3, -6, -10, -13, -17, -20, -24, -27,
                            -30, -34, -37, -40, -43, -46, -49, -52, -55, -58, -61, -64, -66, -69, -71, -74, -76, -78,
                            -80, -82, -84, -86, -88, -89, -91, -92, -93, -95, -96, -97, -97, -98, -99, -99, -99, -99,
                            -100, -99, -99, -99, -99, -98, -97, -97, -96, -95, -93, -92, -91, -89, -88, -86, -84, -82,
                            -80, -78, -76, -74, -71, -69, -66, -64, -61, -58, -55, -52, -50, -46, -43, -40, -37, -34,
                            -30, -27, -24, -20, -17, -13, -10, -6, -3, 0, 3, 6, 10, 13, 17, 20, 24, 27,
                            30, 34, 37, 40, 43, 46, 50, 52, 55, 58, 61, 64, 66, 69, 71, 74, 76, 78, 80, 82,
                            84, 86, 88, 89, 91, 92, 93, 95, 96, 97, 97, 98, 99, 99, 99, 99};

extern Color BLACKCOLOR;
extern Color WHITECOLOR;
extern Color REDCOLOR;
extern Color GREENCOLOR;

extern Image alphanumfont;
extern Image allGameSprite;
extern Image allGameAlphaSprite;
extern Image menuSprite;
#endif