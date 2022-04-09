#include "settingsscreen.h"

SettingsScreen::SettingsScreen(void (*rcb)(int8_t menu, uint8_t option), void (*hscb)(uint32_t highscore), uint32_t highscore, uint8_t option) {
    printf("About screen loading...");
    this->screenId = 5;
    this->type = Type::GAME;
    this->returnCallBack = rcb;
    this->highScoreCallBack = hscb;
    this->option = option;
    this->scroll = 0;

    uint16_t tileWidth = 32, tileHeight = 16;
    uint8_t xCount = (320 / tileWidth);
    uint8_t yCount = (240 / tileHeight);
    uint8_t *ts = new uint8_t[xCount*yCount];
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
    tinfo.color = Color(211, 156, 180);
    tinfo.type = COLORFILL;
    this->bgLayer->addTileInfo(1, tinfo);
    tinfo.color = Color(198, 151, 178);
    this->bgLayer->addTileInfo(2, tinfo);
    printf("Done\n");
}

SettingsScreen::~SettingsScreen() {
}

void SettingsScreen::update(uint16_t deltaTimeMS) {
    uint16_t timeDiff = getTimeDiffMS(this->lastUpdate);
    if(timeDiff > 20) {
        this->scroll++;
        if(this->scroll >= 800)
            this->scroll = 0;
        this->lastUpdate = getTime();
    }
}

void SettingsScreen::draw(Display *display) {
    this->bgLayer->draw(display, 0, 0);    
}

void SettingsScreen::keyPressed(uint8_t key) {
    if(key == KEY_B)
        this->returnCallBack(this->screenId, this->option);
}

void SettingsScreen::keyReleased(uint8_t key) {
}

void SettingsScreen::keyDown(uint8_t key){
}
