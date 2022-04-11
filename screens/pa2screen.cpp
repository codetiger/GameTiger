#include "pa2screen.h"
#include "pa2/pa2level01.h"

PixelAdventureScreen::PixelAdventureScreen(void (*rcb)(int8_t menu, uint8_t option), void (*hscb)(uint32_t highscore), uint32_t highscore, uint8_t option) {
    printf("About screen loading...");
    this->screenId = 5;
    this->type = Type::GAME;
    this->returnCallBack = rcb;
    this->highScoreCallBack = hscb;
    this->option = option;
    this->scroll = 63;
    this->gameScrollX = 0;
    this->gameScrollY = 0;

    uint16_t tileWidth = 64, tileHeight = 64;
    uint8_t xCount = (DISPLAY_WIDTH / tileWidth);
    uint8_t yCount = (DISPLAY_HEIGHT / tileHeight) + 2;
    uint8_t *ts = new uint8_t[xCount*yCount];
    for (int i = 0; i < xCount*yCount; i++)
        ts[i] = 1;
    this->bgLayer = new TileMap(xCount, yCount, tileWidth, tileHeight, ts);

    TileInfo tinfo;
    tinfo.type = SPRITE;
    tinfo.sprite = &pa2bg;
    tinfo.textureID = rand() % 6;
    this->bgLayer->addTileInfo(1, tinfo);
    
    this->gameLayer = new TileMap(32, 18, 16, 16, (uint8_t*)level01);
    tinfo.type = EMPTY;
    this->gameLayer->addTileInfo(0, tinfo);
    for (int i = 1; i < 32*18; i++) {
        TileInfo tinfo;
        tinfo.type = SPRITE;
        tinfo.sprite = &pa2terrain;
        tinfo.textureID = i-1;
        this->gameLayer->addTileInfo(i, tinfo);
    }

    printf("Done\n");
}

PixelAdventureScreen::~PixelAdventureScreen() {
}

void PixelAdventureScreen::update(uint16_t deltaTimeMS) {
    uint16_t timeDiff = getTimeDiffMS(this->lastUpdate);
    if(timeDiff > 50) {
        if(this->scroll == 0)
            this->scroll = 63;
        else
            this->scroll--;
        this->lastUpdate = getTime();
    }
}

void PixelAdventureScreen::draw(Display *display) {
    this->bgLayer->draw(display, 0, this->scroll);
    this->gameLayer->draw(display, this->gameScrollX, this->gameScrollY);
}

void PixelAdventureScreen::keyPressed(uint8_t key) {
    const uint8_t delta = 8;
    if(key == KEY_RIGHT) {
        if(this->gameScrollX < (this->gameLayer->width - DISPLAY_WIDTH) - delta)
            this->gameScrollX += delta;
        else 
            this->gameScrollX = this->gameLayer->width - DISPLAY_WIDTH;
    } else if(key == KEY_LEFT) {
        if(this->gameScrollX > delta)
            this->gameScrollX -= delta;
        else 
            this->gameScrollX = 0;
    } else if(key == KEY_UP) {
        if(this->gameScrollY > delta)
            this->gameScrollY -= delta;
        else 
            this->gameScrollY = 0;
    } else if(key == KEY_DOWN) {
        if(this->gameScrollY < (this->gameLayer->height - DISPLAY_HEIGHT) - delta)
            this->gameScrollY += delta;
        else 
            this->gameScrollY = this->gameLayer->height - DISPLAY_HEIGHT;
    }

    printf("Game x: %d y: %d\n", this->gameScrollX, this->gameScrollY);

    if(key == KEY_B)
        this->returnCallBack(this->screenId, this->option);
}

void PixelAdventureScreen::keyReleased(uint8_t key) {
}

void PixelAdventureScreen::keyDown(uint8_t key) {
}
