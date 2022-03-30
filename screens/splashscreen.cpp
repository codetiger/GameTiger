#include "splashscreen.h"

SplashScreen::SplashScreen(void (*rcb)(int8_t menu), void (*hscb)(uint32_t highscore), uint32_t highscore) {
    printf("Splash screen loading...");
    this->screenId = 0;
    this->type = Type::SPLASH;
    this->returnCallBack = rcb;
    this->highScoreCallBack = hscb;
    this->imageAlpha = 0;
    tiger.setAlpha(this->imageAlpha);
    font.setAlpha(this->imageAlpha);
    this->accDeltaTimeMS = 0;
    this->totalDuration = 0;
    printf("Done\n");
}

SplashScreen::~SplashScreen() {
}

void SplashScreen::update(uint16_t deltaTimeMS) {
    this->totalDuration += deltaTimeMS;
    if(this->totalDuration > 3000) {
        this->returnCallBack(-1);
        return;
    }

    this->accDeltaTimeMS += deltaTimeMS;
    uint8_t frameQuotient = accDeltaTimeMS / 16;
    this->accDeltaTimeMS -= 16 * frameQuotient;

    this->tileMoveX += frameQuotient;
    if(this->tileMoveX >= 80)
        this->tileMoveX = 0;
    this->tileMoveY = 40 * sin(this->tileMoveX * 2 * M_PI / 80);
    if(this->imageAlpha < 255 - frameQuotient * 3)
        this->imageAlpha += frameQuotient * 3;    
    tiger.setAlpha(this->imageAlpha);
    font.setAlpha(this->imageAlpha);
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
    tiger.draw(display, 92, 56);
    font.drawSprites(display, "GAMETIGER", 98, 190);
}

void SplashScreen::keyPressed(uint8_t key) {
    const char c[6] = {'U', 'D', 'L', 'R', 'A', 'B'};
    printf("Key: %c\n", c[key]);
}

void SplashScreen::keyReleased(uint8_t key) {
}

void SplashScreen::keyDown(uint8_t key){
}
