#include "common.h"
#include "display.h"
#include "screen.h"

#ifndef _GAME_TIGER_KEYBOARD_H
#define _GAME_TIGER_KEYBOARD_H

#define ADDR 0x27
#define I2CSDA 2
#define I2CSCL 3

#define KEY_UP 0
#define KEY_DOWN 1
#define KEY_LEFT 2
#define KEY_RIGHT 3
#define KEY_A 4
#define KEY_B 5
#define KEY_EXIT 6
#define KEY_COUNT 7

class KeyBoard
{
private:
    const uint8_t pinId[KEY_COUNT] = {11, 8, 9, 10, 6, 7, 5};
    bool prevKeyState[KEY_COUNT];
public:
    KeyBoard();
    ~KeyBoard();

    void checkKeyState(Screen *screen);
};

#endif