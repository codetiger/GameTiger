#include "splashscreen.h"

SplashScreen::SplashScreen(void (*rcb)(int8_t menu, uint8_t option), void (*hscb)(uint32_t highscore), uint32_t highscore, uint8_t option) {
    printf("[SplashScreen] loading...\n");
    this->screenId = ScreenEnum::SPLASHSCREEN;
    this->returnCallBack = rcb;
    this->highScoreCallBack = hscb;
    this->imageAlpha = 0;
    this->accDeltaTimeMS = 0;
    this->totalDuration = 0;
    this->option = option;

    uint16_t tileWidth = 20, tileHeight = 20;
    uint8_t xCount = 4 + (DISPLAY_WIDTH / tileWidth);
    uint8_t yCount = 4 + (DISPLAY_HEIGHT / tileHeight);
    uint16_t *ts = new uint16_t[xCount*yCount];
    bool colorFlip = true;
    for (int y = 0; y < yCount; y++) {
        for (int x = 0; x < xCount; x++) {
            colorFlip = !colorFlip;
            ts[y*xCount+x] = colorFlip ? 1 : 2;
        }
        colorFlip = !colorFlip;
    }
    this->bgLayer = new TileMap(xCount, yCount, tileWidth, tileHeight, ts);

    TileInfo tinfo; 
    tinfo.color = Color(158, 228, 254);
    tinfo.type = COLORFILL;
    this->bgLayer->addTileInfo(1, tinfo);
    tinfo.color = Color(135, 220, 255);
    this->bgLayer->addTileInfo(2, tinfo);
    printf("[SplashScreen] Done\n");
}

SplashScreen::~SplashScreen() {
    printf("[SplashScreen] Destructing\n");
}

void SplashScreen::update(uint16_t deltaTimeMS) {
    this->totalDuration += deltaTimeMS;
    if(this->totalDuration > 3000) {
        this->returnCallBack(this->screenId, this->option);
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
}

void SplashScreen::draw(Display *display) {
    this->bgLayer->draw(display, this->tileMoveX, this->tileMoveY+40);
    uint16_t width = menuSprite.getSpriteWidth(tigerFrame);
    menuSprite.drawSprite(display, tigerFrame, Vec2((DISPLAY_WIDTH - width)/2, 56), this->imageAlpha);
    std::string title = "GameTiger";
    width = alphanumfont.getTextWidth(title, 2);
    alphanumfont.drawText(display, title, Vec2((DISPLAY_WIDTH - width)/2, 190), this->imageAlpha, 2);
}

void SplashScreen::keyPressed(uint8_t key) {
    const char c[6] = {'U', 'D', 'L', 'R', 'A', 'B'};
    printf("[SplashScreen] Key: %c\n", c[key]);
}

void SplashScreen::keyReleased(uint8_t key) {
}

void SplashScreen::keyDown(uint8_t key){
}
