#include "splashscreen.h"
#include "../content/tiger.h"
#include "../content/font.h"

SplashScreen::SplashScreen(void (*rcb)(int8_t menu), void (*hscb)(uint32_t highscore), uint32_t highscore) {
    this->screenId = 0;
    this->type = Type::SPLASH;
    this->returnCallBack = rcb;
    this->highScoreCallBack = hscb;
    this->imageAlpha = 0;
    this->tiger = new Image(tiger_img_width, tiger_img_height, tiger_color_count, (uint8_t*)tiger_palette, (uint8_t*)tiger_pixel_data);
    this->font = new Image(font_img_width, font_img_height, font_color_count, (uint8_t*)font_palette, (uint8_t*)font_pixel_data, font_sprite_data);
    this->tiger->setAlpha(this->imageAlpha);
    this->font->setAlpha(this->imageAlpha);
    this->startTime = TimeSinceBoot;
}

SplashScreen::~SplashScreen() {
}

void SplashScreen::update() {
    this->tileMoveX += 3;
    if(this->tileMoveX >= 80)
        this->tileMoveX = 0;
    this->tileMoveY = 40 * sin(this->tileMoveX * 2 * M_PI / 80);
    if(this->imageAlpha < 255)
        this->imageAlpha += 15;
    this->tiger->setAlpha(this->imageAlpha);
    this->font->setAlpha(this->imageAlpha);
    uint8_t timeDiffSec = (TimeSinceBoot - this->startTime) / CLOCKS_PER_SEC;
    if(timeDiffSec > 3)
        this->returnCallBack(-1);
}

void SplashScreen::draw(Display *display) {
    Color light(158, 228, 254), dark(135, 220, 255);
    bool colorFlip = true;
    for (int x = -80; x < display->width + 80; x+=20) {
        for (int y = -80; y < display->height + 80; y+=20) {
            colorFlip = !colorFlip;
            display->fillRect(x+tileMoveX, y+tileMoveY, 20, 20, colorFlip ? light : dark);
        }
        colorFlip = !colorFlip;
    }
    this->tiger->draw(display, 92, 56);
    this->font->drawSprites(display, "GAMETIGER", 98, 190);
}

void SplashScreen::keyPressed(uint8_t key) {
    const char c[6] = {'U', 'D', 'L', 'R', 'A', 'B'};
    printf("Key: %c\n", c[key]);
}

void SplashScreen::keyReleased(uint8_t key) {
}

void SplashScreen::keyDown(uint8_t key){
}
