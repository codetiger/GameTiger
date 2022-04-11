#include "common.h"
#include <map>

#ifndef _GAME_TIGER_LEVEL_H
#define _GAME_TIGER_LEVEL_H

enum ItemType {COLLECT, HURT};

struct GameItem {
    ItemType type;
    Image *sprite;
    uint8_t numFrames;
    char *animationFrames;
};


#endif