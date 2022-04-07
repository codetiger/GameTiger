#include "tilemap.h"

TileMap::TileMap(uint8_t xc, uint8_t yc, uint8_t tw, uint8_t th, uint8_t* ts) {
    this->xCount = xc;
    this->yCount = yc;
    this->tileWidth = tw;
    this->tileHeight = th;
    this->tiles = ts;
}

TileMap::~TileMap() {
}

void TileMap::addTileInfo(uint8_t index, TileInfo tinfo) {
    this->tileInfoList.insert(std::pair<uint8_t, TileInfo>(index, tinfo));
}

void TileMap::draw(Display *display, uint16_t screenx, uint16_t screeny) {
    for (int y = 0; y < yCount; y++) {
        int16_t ty = y * this->tileHeight - screeny;
        if(ty + tileHeight >= 0 && ty < display->height) {
            for (int x = 0; x < xCount; x++) {
                int16_t tx = x * this->tileWidth - screenx;
                if(tx + tileWidth >= 0 && tx < display->width) {
                    uint8_t index = this->tiles[y*xCount+x];
                    TileInfo tinfo = this->tileInfoList[index];
                    if(tinfo.type == COLORFILL) {
                        display->fillRect(tx, ty, this->tileWidth, this->tileHeight, tinfo.color);
                    } else if(tinfo.type == SPRITE) {
                        tinfo.sprite->drawSprite(display, tinfo.textureID, tx, ty);
                    }
                }
            }
        }
    }
}
