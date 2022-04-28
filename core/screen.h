#include "common.h"
#include "display.h"
#include "audio.h"

#ifndef _GAME_TIGER_SCREEN_H
#define _GAME_TIGER_SCREEN_H

enum Type { SPLASH, MENU, GAME };

class Screen {
protected:
    void (*returnCallBack)(int8_t menu, uint8_t option);
    void (*highScoreCallBack)(uint32_t highscore);
public:
    virtual void update(uint16_t deltaTimeMS) = 0;
    virtual void draw(Display *display) = 0;
    virtual void keyPressed(uint8_t key) = 0;
    virtual void keyReleased(uint8_t key) = 0;
    virtual void keyDown(uint8_t key) = 0;

    Type type;
    uint8_t screenId = 0;
    AudioController *audioController;
};

#endif