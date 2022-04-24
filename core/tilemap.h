#include "common.h"
#include <map>

#ifndef _GAME_TIGER_TILEMAP_H
#define _GAME_TIGER_TILEMAP_H

enum TileType {EMPTY, COLORFILL, SPRITE, ANIMATEDSPRITE};

struct TileInfo {
    uint16_t textureID;
    TileType type;
    Color color;
    Image *sprite;
    uint8_t numFrames;
    char *animationFrames;
};

class TileMap
{
private:
    uint8_t xCount, yCount;
    uint8_t tileWidth, tileHeight;
    uint16_t *tiles;
    std::map<uint16_t, TileInfo> tileInfoList;
    std::map<uint16_t, uint8_t> animationFrame;
public:
    uint16_t width, height;
    
    TileMap(uint8_t xc, uint8_t yc, uint8_t tw, uint8_t th, uint16_t *ts);
    ~TileMap();
    void addTileInfo(uint16_t index, TileInfo tinfo);
    bool isEmptyTile(uint16_t x, uint16_t y);
    void draw(Display *display, uint16_t screenx, uint16_t screeny);
    void update(uint16_t deltaTimeMS);
};

#endif