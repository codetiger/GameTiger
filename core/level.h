#include "common.h"
#include <map>
#include <vector>
#include <algorithm>
#include "../core/keyboard.h"

#ifndef _GAME_TIGER_LEVEL_H
#define _GAME_TIGER_LEVEL_H

enum GameItemState {IDLE, MOVING, HIT, DISAPPEAR};
enum GameItemMoveType {STATIC, HORIZONTAL, VERTICAL};
enum GameItemType {FRIEND, ENEMY};

struct GameItem {
    uint8_t width, height;
    uint8_t speed;
    int8_t deltaHealth, deltaScore;
    Image *sprite;
    GameItemType type;
    GameItemMoveType movementType;
    GameItemState state;
    uint8_t numIdleFrames, numHitFrames, numRunFrames;
    uint16_t *idleSeq, *hitSeq, *runSeq;

    bool direction;
    uint8_t curFrameIndex;
    uint16_t x, y, minAxis, maxAxis;
};

enum HeroState {STANDING, WALKING, JUMPING, DOUBLEJUMPING, FALLING, HURT};

struct Hero {
    uint8_t width, height;
    Image *sprite;
    HeroState state;
    uint8_t numStandingFrames, numHurtFrames, numWalkFrames, jumpFrame, fallFrame, numDoubleJumpFrames;
    uint16_t *standingSeq, *hurtSeq, *walkSeq, *doubleJumpSeq;

    bool direction;
    uint8_t curFrameIndex, inertia;
    uint16_t x, y;
};

class Level {
private:
    TileMap *bgLayer, *gameLayer;
    uint8_t bgScroll, bgMaxScroll;
    bool isHorizontalScroll;
    uint16_t gameScrollX, gameScrollY;
    timetype lastUpdate;
    std::vector<GameItem> gameItems;

    const uint8_t JUMPINERTIA = 6;

    bool checkGameItemCollision(GameItem &item);
    void updateHeroCollision();
    bool checkHeroMovable(uint8_t direction, int8_t delta);
    void updateGameItem(GameItem &gi);
    void updateHero();
    void updateScreenScroll();
public:
    Hero hero;
    AudioController * audioController;

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