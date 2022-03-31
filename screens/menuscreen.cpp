#include "menuscreen.h"
#include "../core/keyboard.h"

MenuScreen::MenuScreen(void (*rcb)(int8_t menu), void (*hscb)(uint32_t highscore), uint32_t highscore) {
    printf("Menu screen loading...");
    this->screenId = 1;
    this->type = Type::MENU;
    this->returnCallBack = rcb;
    this->highScoreCallBack = hscb;
    this->currentMenuItem = highscore;
    this->selectedMenuItem = highscore;
    this->isAnimating = false;
    this->animationCounter = 0;
    printf("Done\n");
}

MenuScreen::~MenuScreen() {
}

void MenuScreen::update(uint16_t deltaTimeMS) {
    if(this->isAnimating) {
        this->animationCounter += 16;
        if(this->animationCounter > this->menuItemGap) {
            this->animationCounter = 0;
            this->isAnimating = false;
            this->currentMenuItem = this->selectedMenuItem;
        }
    }
}

void MenuScreen::draw(Display *display) {
    Color light(253, 201, 48), dark(252, 183, 35);
    bool colorFlip = true;
    for (int x = 0; x < display->width; x+=40) {
        for (int y = 0; y < display->height; y+=40) {
            colorFlip = !colorFlip;
            display->fillRect(x, y, 40, 40, colorFlip ? light : dark);
        }
        colorFlip = !colorFlip;
    }
    for (int i = 0; i < menuCount; i++) {
        uint8_t alpha = (i == this->currentMenuItem) ? 255 : 64;
        menuItemLogo[i].setAlpha(alpha);
        font.setAlpha(alpha);

        int posx = (i-this->currentMenuItem) * this->menuItemGap;
        if(isAnimating)
            posx += (this->currentMenuItem - this->selectedMenuItem) * this->animationCounter;

        uint16_t width = font.getWidth(this->menuItemNames[i]);
        font.drawSprites(display, this->menuItemNames[i], posx + (display->width - width)/2, 170);
        menuItemLogo[i].draw(display, posx + (display->width - menuItemLogo[i].width)/2, 64);
    }
}

void MenuScreen::keyPressed(uint8_t key) {
    const char c[6] = {'U', 'D', 'L', 'R', 'A', 'B'};
    printf("Key Pressed: %c\n", c[key]);
    if(this->isAnimating)
        return;

    if(key == KEY_RIGHT) {
        if(this->selectedMenuItem < this->menuCount-1)
            this->selectedMenuItem++;
        this->isAnimating = true;
    } else if (key == KEY_LEFT) {
        if(this->selectedMenuItem != 0)
            this->selectedMenuItem--;
        this->isAnimating = true;
    } else if(key == KEY_A) {
        this->returnCallBack(this->currentMenuItem);
    }
}

void MenuScreen::keyReleased(uint8_t key) {
    const char c[6] = {'U', 'D', 'L', 'R', 'A', 'B'};
    printf("Key Released: %c\n", c[key]);
}

void MenuScreen::keyDown(uint8_t key){
    const char c[6] = {'U', 'D', 'L', 'R', 'A', 'B'};
    printf("Key Down: %c\n", c[key]);
}
