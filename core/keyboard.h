#include "pico/stdlib.h"
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

class KeyBoard
{
private:
    const uint8_t gpioPins[6] = {2, 0, 1, 3, 4, 5};
    bool prevKeyState[6];
public:
    KeyBoard();
    ~KeyBoard();

    void checkKeyState(Screen *screen);
};

#endif