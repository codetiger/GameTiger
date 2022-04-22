#include "level.h"

Level::Level() {
    this->bgScroll = 0;
    this->gameScrollX = 0;
    this->gameScrollY = 0;
    this->bgLayer = 0;
    this->gameLayer = 0;
}

Level::~Level() {
}

void Level::update(uint16_t deltaTimeMS) {
    uint16_t timeDiff = getTimeDiffMS(this->lastUpdate);
    if(timeDiff > 50) {
        if(this->bgScroll == 0)
            this->bgScroll = this->bgMaxScroll;
        else
            this->bgScroll--;

        for(GameItem &item : gameItems) {
            if(item.state != PICKED)
                item.curFrameIndex++;
            else 
                continue;

            if(item.state == HIT && item.curFrameIndex >= item.numHitFrames) {
                item.curFrameIndex = 0;
                item.state = PICKED;
            } else if(item.state == FRESH && item.curFrameIndex >= item.numIdleFrames)
                item.curFrameIndex = 0;
        }
        this->lastUpdate = getTime();
    }
}

void Level::draw(Display *display) {
    if(this->bgLayer)
        this->bgLayer->draw(display, this->isHorizontalScroll ? this->bgScroll : 0, this->isHorizontalScroll ? 0 : this->bgScroll);
    if(this->gameLayer)
        this->gameLayer->draw(display, this->gameScrollX, this->gameScrollY);
    for(GameItem item : gameItems) {
        if(item.state != PICKED) {
            uint16_t frame = (item.state == FRESH) ? item.idleSeq[item.curFrameIndex] : item.hitSeq[item.curFrameIndex];
            item.sprite->drawSprite(display, frame, item.x-this->gameScrollX, item.y-this->gameScrollY);
        }
    }
}

void Level::setBGLayer(Image *sprite, uint16_t spriteID, bool isHSroll) {
    uint16_t tileWidth = sprite->getSpriteWidth(spriteID);
    uint16_t tileHeight = sprite->getSpriteHeight(spriteID);
    this->bgMaxScroll = tileWidth;
    this->isHorizontalScroll = isHSroll;
    uint8_t xCount = (DISPLAY_WIDTH / tileWidth) + 2;
    uint8_t yCount = (DISPLAY_HEIGHT / tileHeight) + 2;
    uint16_t *ts = new uint16_t[xCount*yCount];
    for (int i = 0; i < xCount*yCount; i++)
        ts[i] = 1;
    this->bgLayer = new TileMap(xCount, yCount, tileWidth, tileHeight, ts);

    TileInfo tinfo;
    tinfo.type = SPRITE;
    tinfo.sprite = sprite;
    tinfo.textureID = spriteID;
    this->bgLayer->addTileInfo(1, tinfo);
}

void Level::setGameLayer(Image *sprite, uint8_t xCount, uint8_t yCount, uint16_t *ts, uint16_t emptyTileIndex) {
    uint8_t tileWidth = sprite->getSpriteWidth(ts[0]);
    uint8_t tileHeight = sprite->getSpriteHeight(ts[0]);
    this->gameLayer = new TileMap(xCount, yCount, tileWidth, tileHeight, ts);
    for (int i = 0; i < xCount*yCount; i++) {
        TileInfo tinfo;
        tinfo.type = (ts[i] != emptyTileIndex) ? SPRITE : EMPTY;
        tinfo.sprite = sprite;
        tinfo.textureID = ts[i];
        this->gameLayer->addTileInfo(ts[i], tinfo);
    }
}

void Level::addGoodie(uint16_t x, uint16_t y, Image *sprite, uint8_t dScore, uint8_t dHealth, uint8_t nIdleFrames, uint16_t *idleSeq, uint8_t nHitFrames, uint16_t *hitSeq) {
    GameItem item;
    item.state = FRESH;
    item.type = GOODIE;
    item.x = x;item.y = y;
    item.width = sprite->getSpriteWidth(idleSeq[0]);
    item.height = sprite->getSpriteHeight(idleSeq[0]);
    item.sprite = sprite;
    item.deltaHealth = dHealth;
    item.deltaScore = dScore;
    item.numIdleFrames = nIdleFrames;
    item.idleSeq = idleSeq;
    item.numHitFrames = nHitFrames;
    item.hitSeq = hitSeq;
    item.curFrameIndex = 0;
    gameItems.push_back(item);
}

void Level::keyPressed(uint8_t key) {
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
}

void Level::keyReleased(uint8_t key) {

}

void Level::keyDown(uint8_t key) {

}
