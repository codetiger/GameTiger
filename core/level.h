#include "common.h"
#include <map>
#include <vector>
#include "../core/keyboard.h"

#ifndef _GAME_TIGER_LEVEL_H
#define _GAME_TIGER_LEVEL_H

enum GameItemState {IDLE, MOVING, HIT, DEAD};
enum GameItemMoveType {STATIC, HORIZONTAL, VERTICAL};

struct GameItem {
    uint8_t width, height;
    uint8_t speed;
    int8_t deltaHealth, deltaScore;
    Image *sprite;
    GameItemMoveType movementType;
    GameItemState state;
    uint8_t numIdleFrames, numHitFrames, numRunFrames;
    uint16_t *idleSeq, *hitSeq, *runSeq;

    bool direction;
    uint8_t curFrameIndex;
    uint16_t x, y, minAxis, maxAxis;
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
    void addGameItem(GameItem &gi);

    void update(uint16_t deltaTimeMS);
    void draw(Display *display);
    void keyPressed(uint8_t key);
    void keyReleased(uint8_t key);
    void keyDown(uint8_t key);
};

#endif