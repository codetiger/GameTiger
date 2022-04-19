#include "common.h"
#include <map>
#include <vector>
#include "../core/keyboard.h"

#ifndef _GAME_TIGER_LEVEL_H
#define _GAME_TIGER_LEVEL_H

enum ItemType {GOODIE, HURT};
enum ItemState {FRESH, HIT, PICKED};

struct GameItem {
    uint16_t x, y;
    uint8_t width, height;
    ItemType type;
    ItemState state;
    Image *sprite;
    uint8_t curFrameIndex;
    uint8_t deltaScore, deltaHealth;
    uint8_t numIdleFrames, numHitFrames;
    uint16_t *idleSeq, *hitSeq;
};

enum EnemyState {IDLE, MOVING, FOLLOWING, SHOOTING, HITKILL, DEAD};

struct Enemy {
    uint16_t x, y;
    uint8_t width, height;
    EnemyState state;
    Image *sprite;
    uint8_t curFrameIndex;
    uint8_t deltaHealth;
    uint8_t numIdleFrames, numHitFrames;
};

class Level {
private:
    TileMap *bgLayer, *gameLayer;
    uint8_t bgScroll, bgMaxScroll;
    bool isHorizontalScroll;
    uint16_t gameScrollX, gameScrollY;
    timetype lastUpdate;
    std::vector<GameItem> gameItems;
public:
    Level();
    ~Level();

    void setBGLayer(Image *sprite, uint16_t spriteID, bool isHorizontalScroll);
    void setGameLayer(Image *sprite, uint8_t xCount, uint8_t yCount, uint16_t *ts, uint16_t emptyTileIndex);
    void addGoodie(uint16_t x, uint16_t y, Image *sprite, uint8_t deltaScore, uint8_t deltaHealth, uint8_t numIdleFrames, uint16_t *idleSeq, uint8_t numHitFrames, uint16_t *hitSeq);

    void update(uint16_t deltaTimeMS);
    void draw(Display *display);
    void keyPressed(uint8_t key);
    void keyReleased(uint8_t key);
    void keyDown(uint8_t key);
};

#endif