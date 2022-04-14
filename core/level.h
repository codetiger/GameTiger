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
    uint8_t idleSeqIndex, hitSeqIndex;
    const char (*animSeq)[17];
};

// enum EnemyState {IDLE, MOVING, FOLLOWING, SHOOTING, HIT};

struct Enemy {
    uint16_t x, y;
    // EnemyState state;
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

    void setBGLayer(Image *sprite, char spriteID, bool isHorizontalScroll);
    void setGameLayer(Image *sprite, uint8_t xCount, uint8_t yCount, uint8_t *ts);
    void addGoodie(uint16_t x, uint16_t y, Image *sprite, uint8_t deltaScore, uint8_t deltaHealth, uint8_t numIdleFrames, uint8_t idleSeqIndex, uint8_t numHitFrames, uint8_t hitSeqIndex, const char (*animSeq)[17]);

    void update(uint16_t deltaTimeMS);
    void draw(Display *display);
    void keyPressed(uint8_t key);
    void keyReleased(uint8_t key);
    void keyDown(uint8_t key);
};

#endif