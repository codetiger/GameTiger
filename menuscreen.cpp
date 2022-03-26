#include "pico/stdlib.h"
#include "menuscreen.h"
#include "content/font.h"
#include "content/snake.h"
#include "content/tetris.h"
#include "content/g2048.h"
#include "core/keyboard.h"

MenuScreen::MenuScreen(void (*rcb)(int8_t menu), void (*hscb)(uint32_t highscore), uint32_t highscore) {
    this->screenId = 1;
    this->type = Type::MENU;
    this->returnCallBack = rcb;
    this->highScoreCallBack = hscb;
    this->currentMenuItem = this->selectedMenuItem = 0;
    this->isAnimating = false;
    this->animationCounter = 0;
    this->font = new Image(font_img_width, font_img_height, font_color_count, (uint8_t*)font_palette, (uint8_t*)font_pixel_data, font_sprite_data);
    this->menuItemLogo[0] = new Image(snake_img_width, snake_img_height, snake_color_count, (uint8_t*)snake_palette, (uint8_t*)snake_pixel_data);
    this->menuItemLogo[1] = new Image(g2048_img_width, g2048_img_height, g2048_color_count, (uint8_t*)g2048_palette, (uint8_t*)g2048_pixel_data);
    this->menuItemLogo[2] = new Image(tetris_img_width, tetris_img_height, tetris_color_count, (uint8_t*)tetris_palette, (uint8_t*)tetris_pixel_data);
    this->menuItemLogo[3] = new Image(snake_img_width, snake_img_height, snake_color_count, (uint8_t*)snake_palette, (uint8_t*)snake_pixel_data);
    this->menuItemLogo[4] = new Image(snake_img_width, snake_img_height, snake_color_count, (uint8_t*)snake_palette, (uint8_t*)snake_pixel_data);
}

MenuScreen::~MenuScreen() {
}

void MenuScreen::update() {
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
    for (int i = 0; i < 5; i++) {
        int posx = (i-this->currentMenuItem) * this->menuItemGap;
        posx += (display->width - this->menuItemLogo[i]->width)/2;
        if(isAnimating)
            posx += (this->currentMenuItem - this->selectedMenuItem) * this->animationCounter;
        uint8_t alpha = (i == this->currentMenuItem) ? 255 : 64;
        this->menuItemLogo[i]->setAlpha(alpha);
        this->font->setAlpha(alpha);
        this->menuItemLogo[i]->draw(display, posx, 64);
        this->font->drawSprites(display, this->menuItemNames[i], posx + 10, 170);
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
