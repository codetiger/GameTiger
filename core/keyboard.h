#include "common.h"
#include "display.h"
#include "screen.h"

#ifndef _GAME_TIGER_KEYBOARD_H
#define _GAME_TIGER_KEYBOARD_H

#define KEY_UP 0
#define KEY_DOWN 1
#define KEY_LEFT 2
#define KEY_RIGHT 3
#define KEY_A 4
#define KEY_B 5
#define KEY_START 6
#define KEY_SELECT 7
#define KEY_COUNT 8

class KeyBoard
{
private:
    const uint8_t pinId[KEY_COUNT] = {17, 20, 19, 18, 6, 7, 8, 9};
    bool prevKeyState[KEY_COUNT];
public:
    KeyBoard();
    ~KeyBoard();

    void checkKeyState(Screen *screen);
};

#endif