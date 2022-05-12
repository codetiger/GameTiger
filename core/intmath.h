#include "common.h"

#ifndef _GAME_TIGER_INTMATH_H
#define _GAME_TIGER_INTMATH_H

#define FRACTIONS_PER_UNIT 512
#define Unit int16_t
#define Index uint16_t
#define UNIT_MAX 32767
#define UNIT_MIN -32768

#define MAX_TRIANGES_DRAWN 128

#define HALF_RESOLUTION_X (DISPLAY_WIDTH >> 1)
#define HALF_RESOLUTION_Y (DISPLAY_HEIGHT >> 1)

#define FPU FRACTIONS_PER_UNIT
#define FPU2 (FRACTIONS_PER_UNIT * FRACTIONS_PER_UNIT)

#define SIN_TABLE_LENGTH 128
#define SIN_TABLE_UNIT_STEP (FRACTIONS_PER_UNIT / (SIN_TABLE_LENGTH * 4))

Unit intSqrt(Unit v);
Unit wrap(Unit value, Unit mod);
Unit clamp(Unit v, Unit min, Unit max);
Unit intSin(Unit x);
Unit intASin(Unit x);
Unit intCos(Unit x);

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

    // Vec2 operator/(const float &v) {
    //     return Vec2(this->x / v, this->y / v);
    // }

    int32_t dot(const Vec2 &v) {
        return ((int32_t)x * (int32_t)v.x) + ((int32_t)y * (int32_t)v.y);
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

typedef struct Vec4 {
    Unit x, y, z, w;

    Vec4() {
        w = FRACTIONS_PER_UNIT;
        x = y = z = 0;
    }

    Vec4(Unit _x, Unit _y, Unit _z, Unit _w) {
        w = _w;
        x = _x; y = _y; z = _z;
    }

    Unit length() {
        return intSqrt(x*x + y*y + z*z);
    }

    Vec4& operator+(const Vec4 &v) {
        x += v.x; y += v.y; z += v.z;
        return *this;
    }

    Vec4& operator*(const Unit &v) {
        x *= v; y *= v; z *= v;
        return *this;
    }

    Vec4& operator/(const Unit &v) {
        x /= v; y /= v; z /= v;
        return *this;
    }

    Vec4& operator-(const Vec4 &v) {
        x -= v.x; y -= v.y; z -= v.z;
        return *this;
    }

    Vec4& operator=(const Vec4 &v) {
        x = v.x; y = v.y; z = v.z; w = v.w;
        return *this;
    }

    Unit dot(const Vec4 &v) {
        return (x * v.x + y * v.y + z * v.z + w * v.w);
    }

    Vec4 cross(const Vec4 &v) {
        Vec4 result;
        result.x = y * v.z - z * v.y; 
        result.y = z * v.x - x * v.z; 
        result.z = x * v.y - y * v.x; 
        result.w = 0;
        return result;
    }

    void normalize() {
        Unit l = length();
        if(l == 0) 
            return;

        x = (x * FRACTIONS_PER_UNIT) / l;
        y = (y * FRACTIONS_PER_UNIT) / l;
        z = (z * FRACTIONS_PER_UNIT) / l;
    }

    void perspectiveDivide(Unit focalLength) {
        x = (x * focalLength) / z;
        y = (y * focalLength) / z;
    }

    Vec4 getScreenCoords() {
        Vec4 screenCoords;
        screenCoords.x = HALF_RESOLUTION_X + (x * HALF_RESOLUTION_X) / FRACTIONS_PER_UNIT;
        screenCoords.y = HALF_RESOLUTION_Y - (y * HALF_RESOLUTION_X) / FRACTIONS_PER_UNIT;
        screenCoords.z = z;
        screenCoords.w = w;
        return screenCoords;
    }

    Vec2 xy() {
        return Vec2(x, y);
    }

    void print() {
        printf("%d, %d, %d, %d\n", x, y, z, w);
    }
} Vec4;

typedef struct Transform3D {
    Vec4 translation, rotation, scale;

    Transform3D() {
        scale.x = scale.y = scale.z = FRACTIONS_PER_UNIT; scale.w = 0;
    }

    void setTranslation(Unit tx, Unit ty, Unit tz) {
        translation.x = tx; translation.y = ty; translation.z = tz;
    }

    void setRotation(Unit rx, Unit ry, Unit rz) {
        rotation.x = rx; rotation.y = ry; rotation.z = rz;
    }

    void setScale(Unit sx, Unit sy, Unit sz) {
        scale.x = sx; scale.y = sy; scale.z = sz;
    }

    Transform3D& operator=(const Transform3D &t) {
        translation = t.translation;
        rotation = t.rotation;
        scale = t.scale;
        return *this;
    }

    void print() {
        printf("Translation: %d, %d, %d\n", translation.x, translation.y, translation.z);
        printf("Rotation: %d, %d, %d\n", rotation.x, rotation.y, rotation.z);
        printf("Scale: %d, %d, %d\n", scale.x, scale.y, scale.z);
    }
} Transform3D;

typedef struct Mat4 {
    Unit m[4][4];

    Mat4() {
        m[0][0] = FPU;  m[1][0] = 0;    m[2][0] = 0;    m[3][0] = 0;
        m[0][1] = 0;    m[1][1] = FPU;  m[2][1] = 0;    m[3][1] = 0;
        m[0][2] = 0;    m[1][2] = 0;    m[2][2] = FPU;  m[3][2] = 0;
        m[0][3] = 0;    m[1][3] = 0;    m[2][3] = 0;    m[3][3] = FPU;
    }

    Mat4& makeScaleMat(Vec4 scale) {
        m[0][0] = scale.x; m[1][0] = 0;       m[2][0] = 0;       m[3][0] = 0;
        m[0][1] = 0;       m[1][1] = scale.y; m[2][1] = 0;       m[3][1] = 0;
        m[0][2] = 0;       m[1][2] = 0;       m[2][2] = scale.z; m[3][2] = 0;
        m[0][3] = 0;       m[1][3] = 0;       m[2][3] = 0;       m[3][3] = FPU;
        return *this;
    }

    Mat4& makeTranslationMat(Vec4 offset) {
        m[0][0] = FPU;      m[1][0] = 0;        m[2][0] = 0;        m[3][0] = 0;
        m[0][1] = 0;        m[1][1] = FPU;      m[2][1] = 0;        m[3][1] = 0;
        m[0][2] = 0;        m[1][2] = 0;        m[2][2] = FPU;      m[3][2] = 0;
        m[0][3] = offset.x; m[1][3] = offset.y; m[2][3] = offset.z; m[3][3] = FPU;
        return *this;
    }

    Mat4& makeRotationMat(Vec4 by) {
        by = by * -1;

        Unit sx = intSin(by.x);
        Unit sy = intSin(by.y);
        Unit sz = intSin(by.z);
        Unit cx = intCos(by.x);
        Unit cy = intCos(by.y);
        Unit cz = intCos(by.z);

        #define M(x,y) m[x][y]
        #define S FRACTIONS_PER_UNIT

        M(0,0) = (cy * cz) / S + (sy * sx * sz) / (S * S);
        M(1,0) = (cx * sz) / S;
        M(2,0) = (cy * sx * sz) / (S * S) - (cz * sy) / S;
        M(3,0) = 0;

        M(0,1) = (cz * sy * sx) / (S * S) - (cy * sz) / S;
        M(1,1) = (cx * cz) / S;
        M(2,1) = (cy * cz * sx) / (S * S) + (sy * sz) / S;
        M(3,1) = 0;

        M(0,2) = (cx * sy) / S;
        M(1,2) = -1 * sx;
        M(2,2) = (cy * cx) / S;
        M(3,2) = 0;

        M(0,3) = 0;
        M(1,3) = 0;
        M(2,3) = 0;
        M(3,3) = FRACTIONS_PER_UNIT;
        return *this;
    }

    Mat4& operator=(const Mat4 &v) {
        for (uint8_t j = 0; j < 4; ++j)
            for (uint8_t i = 0; i < 4; ++i)
                this->m[i][j] = v.m[i][j];
        return *this;
    };

    Mat4& transpose() {
        for (uint8_t y = 0; y < 3; ++y)
            for (uint8_t x = 1 + y; x < 4; ++x) {
                Unit tmp = m[x][y];
                m[x][y] = m[y][x];
                m[y][x] = tmp;
            }
        return *this;
    }

    Mat4& operator*(const Mat4 &v) {
        Mat4 mat;
        for (uint8_t row = 0; row < 4; ++row)
            for (uint8_t col = 0; col < 4; ++col) {
                mat.m[col][row] = 0;
                for (uint8_t i = 0; i < 4; ++i)
                    mat.m[col][row] += (m[i][row] * v.m[col][i]) / FRACTIONS_PER_UNIT;
            }
        for (uint8_t j = 0; j < 4; ++j)
            for (uint8_t i = 0; i < 4; ++i)
                this->m[i][j] = mat.m[i][j];
        return *this;
    }

    Vec4 operator*(const Vec4 &v) {
        Vec4 result;
        result.x = (v.x * m[0][0]) / FPU + (v.y * m[0][1]) / FPU + (v.z * m[0][2]) / FPU + m[0][3];
        result.y = (v.x * m[1][0]) / FPU + (v.y * m[1][1]) / FPU + (v.z * m[1][2]) / FPU + m[1][3];
        result.z = (v.x * m[2][0]) / FPU + (v.y * m[2][1]) / FPU + (v.z * m[2][2]) / FPU + m[2][3];
        result.w = FPU;
        return result;
    }

    Mat4& applyTransform(Transform3D transform) {
        makeScaleMat(transform.scale);
        Mat4 r; r.makeRotationMat(transform.rotation);
        *this = *this * r;
        Mat4 t; t.makeTranslationMat(transform.translation);
        *this = *this * t;
        return *this;
    }

    void print() const {
        for (uint8_t row = 0; row < 4; ++row) {
            for (uint8_t col = 0; col < 4; ++col) {
                printf("%d,\t", m[col][row]);
            }
            printf("\n");
        }
    }
} Mat4;

#endif