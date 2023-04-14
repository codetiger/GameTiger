#include "common.h"

#ifndef _GAME_TIGER_INTMATH_H
#define _GAME_TIGER_INTMATH_H

#define Unit int16_t
#define Index uint16_t
#define UNIT_MAX 32767
#define UNIT_MIN -32768

#define HALF_RESOLUTION_X (DISPLAY_WIDTH >> 1)
#define HALF_RESOLUTION_Y (DISPLAY_HEIGHT >> 1)

Unit wrap(Unit value, Unit mod);
Unit clamp(Unit v, Unit min, Unit max);

typedef struct Vec2 {
    Unit x, y;

    Vec2() {
        x = y = 0;
    }

    Vec2(Unit _x, Unit _y) {
        x = _x; y = _y;
    }

    Vec2 operator+(const Unit &v) {
        return Vec2(this->x + v, this->y + v);
    }

    Vec2 operator-(const Vec2 &v) {
        return Vec2(this->x - v.x, this->y - v.y);
    }

    Vec2 operator+(const Vec2 &v) {
        return Vec2(this->x + v.x, this->y + v.y);
    }

    Vec2 operator*(const Unit &v) {
        return Vec2(this->x * v, this->y * v);
    }

    Vec2 operator/(const Unit &v) {
        return Vec2(this->x / v, this->y / v);
    }

    int32_t dot(const Vec2 &v) {
        return (x * v.x) + (y * v.y);
    }

    void print() {
        printf("%d, %d\n", x, y);
    }
} Vec2;

typedef struct Size2 {
    Index w, h;

    Size2() {
        w = h = 0;
    }

    Size2(Index _w, Index _h) {
        w = _w; h = _h;
    }

    Size2 operator*(const Index &v) {
        Size2 result;
        result.w = this->w * v;
        result.h = this->h * v;
        return result;
    };
} Size2;

typedef struct Rect2 {
    Unit x, y;
    Index w, h;

    Rect2() {
        x = y = w = h = 0;
    }

    Rect2(Vec2 _p, Size2 _s) {
        x = _p.x; y = _p.y;
        w = _s.w; h = _s.h;
    }

    Rect2(Unit _x, Unit _y, Index _w, Index _h) {
        x = _x; y = _y;
        w = _w; h = _h;
    }
} Rect2;

#endif