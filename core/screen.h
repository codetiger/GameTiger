#include "pico/stdlib.h"
#include "display.h"

#ifndef _GAME_TIGER_SCREEN_H
#define _GAME_TIGER_SCREEN_H

enum Type { SPLASH, MENU, GAME };

class Screen {
protected:
    void (*returnCallBack)(int8_t menu);
public:
    virtual void update();
    virtual void draw(Display *display);
    virtual void keyPressed(uint8_t key);
    virtual void keyReleased(uint8_t key);
    virtual void keyDown(uint8_t key);

    Type type; 
};

#endif