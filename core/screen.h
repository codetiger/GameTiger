#include "common.h"
#include "display.h"

#ifndef _GAME_TIGER_SCREEN_H
#define _GAME_TIGER_SCREEN_H

enum ScreenEnum { SPLASHSCREEN, MENUSCREEN, SNAKESCREEN, GAMEBOYSCREEN, G2048SCREEN, TETRISSCREEN, MINESCREEN, TICSCREEN, PA2SCREEN, SETTINGSSCREEN, ABOUTSCREEN };

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

    virtual ~Screen() {
        // printf("[Screen] Destructing base screen\n");
    }
    uint8_t screenId = 0;
};

#endif