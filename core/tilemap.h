#include "common.h"
#include <map>

#ifndef _GAME_TIGER_TILEMAP_H
#define _GAME_TIGER_TILEMAP_H

enum TileType {EMPTY, COLORFILL, SPRITE, ANIMATEDSPRITE};

struct TileInfo {
    char textureID;
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
    uint8_t *tiles;
    std::map<uint8_t, TileInfo> tileInfoList;
    std::map<uint8_t, uint8_t> animationFrame;
public:
    uint16_t width, height;
    
    TileMap(uint8_t xc, uint8_t yc, uint8_t tw, uint8_t th, uint8_t *ts);
    ~TileMap();
    void addTileInfo(uint8_t index, TileInfo tinfo);
    void draw(Display *display, uint16_t screenx, uint16_t screeny);
    void update(uint16_t deltaTimeMS);
};

#endif