#include "aboutscreen.h"

AboutScreen::AboutScreen(void (*rcb)(int8_t menu, uint8_t option), void (*hscb)(uint32_t highscore), uint32_t highscore, uint8_t option) {
    printf("[AboutScreen] loading...\n");
    this->screenId = ScreenEnum::ABOUTSCREEN;
    this->returnCallBack = rcb;
    this->highScoreCallBack = hscb;
    this->option = option;
    this->scroll = 0;

    uint16_t tileWidth = 40, tileHeight = 48;
    uint8_t xCount = (DISPLAY_WIDTH / tileWidth);
    uint8_t yCount = (DISPLAY_HEIGHT / tileHeight);
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
    tinfo.color = Color(143, 196, 134);
    tinfo.type = COLORFILL;
    this->bgLayer->addTileInfo(1, tinfo);
    tinfo.color = Color(130, 188, 132);
    this->bgLayer->addTileInfo(2, tinfo);
    printf("[AboutScreen] Done\n");
}

AboutScreen::~AboutScreen() {
}

void AboutScreen::update(uint16_t deltaTimeMS) {
    uint16_t timeDiff = getTimeDiffMS(this->lastUpdate);
    if(timeDiff > 20) {
        this->scroll++;
        if(this->scroll >= 1000)
            this->scroll = 0;
        this->lastUpdate = getTime();
    }
}

void AboutScreen::draw(Display *display) {
    this->bgLayer->draw(display, 0, 0);

    int16_t y = DISPLAY_HEIGHT;
    for (int i = 0; i < 10; i++) {
        std::string title = aboutText[i][0];
        uint16_t width = alphanumfont.getTextWidth(title, 2);
        alphanumfont.drawText(display, title, Vec2((DISPLAY_WIDTH - width)/2, y - scroll), 255, 2);
        y += 36;
        for (int j = 1; j < 5; j++) {
            std::string text = aboutText[i][j];
            if(text.length() > 0) {
                uint16_t width = alphanumfont.getTextWidth(text);
                alphanumfont.drawText(display, text, Vec2((DISPLAY_WIDTH - width)/2, y - scroll));
                y += 24;
            }
        }
        y += 48;
    }
}

void AboutScreen::keyPressed(uint8_t key) {
    if(key == KEY_SELECT)
        this->returnCallBack(this->screenId, this->option);
}

void AboutScreen::keyReleased(uint8_t key) {
}

void AboutScreen::keyDown(uint8_t key){
}
