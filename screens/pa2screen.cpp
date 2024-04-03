#include "pa2screen.h"
#include "../content/pa2levels.h"

PixelAdventureScreen::PixelAdventureScreen(void (*rcb)(int8_t menu, uint8_t option), void (*hscb)(uint32_t highscore), uint32_t highscore, uint8_t option) {
    printf("[PixelAdventureScreen] loading...\n");
    this->screenId = ScreenEnum::PA2SCREEN;
    this->returnCallBack = rcb;
    this->highScoreCallBack = hscb;
    this->option = option;
    this->gameState = PLAYING;
    this->loadLevel(this->option-1);

    printf("[PixelAdventureScreen] Done\n");
}

PixelAdventureScreen::~PixelAdventureScreen() {
}

void PixelAdventureScreen::loadLevel(uint8_t level) {
    this->level = new Level();
    this->level->setBGLayer(&allGameSprite, bgFrames[rand() % 7], false);

    uint8_t xCount = terrainSizes[level*2 + 0];
    uint8_t yCount = terrainSizes[level*2 + 1];
    uint16_t *levelMap = new uint16_t[xCount*yCount];
    uint16_t terrainStartIndex = 0, fruitStartIndex = 0, enemyStartIndex = 0;
    for (uint8_t i = 0; i < level; i++) {
        terrainStartIndex += terrainSizes[i*2 + 0] * terrainSizes[i*2 + 1];
        fruitStartIndex += fruitCounts[i];
        enemyStartIndex += enemyCounts[i];
    }
    
    for (int i = terrainStartIndex; i < terrainStartIndex + xCount*yCount; i++)
        levelMap[i-terrainStartIndex] = terrainFrames[terrainIndex[i]-1];
    this->level->setGameLayer(&allGameSprite, xCount, yCount, levelMap, 24);

    for (int i = fruitStartIndex; i < fruitStartIndex+fruitCounts[level]; i++) {
        uint16_t *anim = 0;
        switch (fruitIndex[i]) {
        case 0:anim = (uint16_t*)AppleAnimSeq;break;
        case 1:anim = (uint16_t*)BananasAnimSeq;break;
        case 2:anim = (uint16_t*)CherriesAnimSeq;break;
        case 3:anim = (uint16_t*)KiwiAnimSeq;break;
        case 4:anim = (uint16_t*)MelonAnimSeq;break;
        case 5:anim = (uint16_t*)OrangeAnimSeq;break;
        case 6:anim = (uint16_t*)PineappleAnimSeq;break;
        case 7:anim = (uint16_t*)StrawberryAnimSeq;break;
        }
        GameItem item;
        item.state = IDLE;
        item.movementType = STATIC_MT;
        item.type = FRIEND;
        item.x = fruitPos[i*2 + 0];
        item.y = fruitPos[i*2 + 1];
        item.width = allGameAlphaSprite.getSpriteWidth(anim[0]);
        item.height = allGameAlphaSprite.getSpriteHeight(anim[0]);
        item.sprite = &allGameAlphaSprite;
        item.deltaHealth = 10;
        item.deltaScore = 0;
        item.numIdleFrames = 17;
        item.idleSeq = anim;
        item.numHitFrames = 6;
        item.hitSeq = (uint16_t*)CollectedAnimSeq;
        item.curFrameIndex = 0;
        this->level->addGameItem(item);
    }

    for (int i = enemyStartIndex; i < enemyStartIndex+enemyCounts[level]; i++) {
        GameItem item;
        item.state = IDLE;
        item.movementType = HORIZONTAL_MT;
        item.type = ENEMY;
        item.x = enemyPos[i*2 + 0];
        item.y = enemyPos[i*2 + 1];
        item.minAxis = enemyLimits[i*2 + 0];
        item.maxAxis = enemyLimits[i*2 + 1];
        item.sprite = &allGameAlphaSprite;
        item.deltaHealth = -100;
        item.deltaScore = 0;
        
        switch (enemyIndex[i]) {
        case 0: 
            item.speed = 2;
            item.numIdleFrames = 14; 
            item.idleSeq = (uint16_t*)mushroomIdleAnimSeq;
            item.numRunFrames = 16;
            item.runSeq = (uint16_t*)mushroomRunAnimSeq;
            item.numHitFrames = 5;
            item.hitSeq = (uint16_t*)mushroomHitAnimSeq;
            break;
        case 1: 
            item.speed = 8;
            item.numIdleFrames = 11; 
            item.idleSeq = (uint16_t*)rinoIdleAnimSeq;
            item.numRunFrames = 6;
            item.runSeq = (uint16_t*)rinoRunAnimSeq;
            item.numHitFrames = 5;
            item.hitSeq = (uint16_t*)rinoHitAnimSeq;
            break;
        }
        item.width = allGameAlphaSprite.getSpriteWidth(item.idleSeq[0]);
        item.height = allGameAlphaSprite.getSpriteHeight(item.idleSeq[0]);
        item.curFrameIndex = 0;
        this->level->addGameItem(item);
    }

    this->level->hero.x = heroPos[level*2 + 0];
    this->level->hero.y = heroPos[level*2 + 1];
    this->level->hero.width = allGameAlphaSprite.getSpriteWidth(heroIdleAnimSeq[0]);
    this->level->hero.height = allGameAlphaSprite.getSpriteHeight(heroIdleAnimSeq[0]);
    this->level->hero.sprite = &allGameAlphaSprite;
    this->level->hero.numStandingFrames = 11;
    this->level->hero.numHurtFrames = 7;
    this->level->hero.numWalkFrames = 12;
    this->level->hero.jumpFrame = heroJumpFrame;
    this->level->hero.fallFrame = heroFallFrame;
    this->level->hero.numDoubleJumpFrames = 7;
    this->level->hero.direction = false;
    this->level->hero.standingSeq = (uint16_t*)heroIdleAnimSeq;
    this->level->hero.hurtSeq = (uint16_t*)heroHitAnimSeq;
    this->level->hero.walkSeq = (uint16_t*)heroRunAnimSeq;
    this->level->hero.doubleJumpSeq = (uint16_t*)heroDoubleJumpAnimSeq;
    this->level->hero.curFrameIndex = 0;
    this->level->hero.state = STANDING;
    this->level->hero.direction = false;
}

void PixelAdventureScreen::update(uint16_t deltaTimeMS) {
    this->level->update(deltaTimeMS);
}

void PixelAdventureScreen::draw(Display *display) {
    this->level->draw(display);

    if(this->gameState == PAUSED) {
        std::string str = "Game Paused";
        uint16_t width = alphanumfont.getTextWidth(str, 2);
        alphanumfont.drawText(display, str, Vec2((DISPLAY_WIDTH - width)/2, 108), 255, 2);
        str = "Press A to continue";
        width = alphanumfont.getTextWidth(str, 1);
        alphanumfont.drawText(display, str, Vec2((DISPLAY_WIDTH - width)/2, 140), 255, 1);
        str = "Press B to quit";
        width = alphanumfont.getTextWidth(str, 1);
        alphanumfont.drawText(display, str, Vec2((DISPLAY_WIDTH - width)/2, 160), 255, 1);
    }
}

void PixelAdventureScreen::keyPressed(uint8_t key) {
    if(key == KEY_SELECT)
        this->returnCallBack(this->screenId, this->option);
    else if(key == KEY_B) {
        if(this->gameState == PAUSED)
            this->gameState = PLAYING;
        else if(this->gameState == PLAYING)
            this->gameState = PAUSED;
    } else if(this->gameState == PLAYING) {
        this->level->keyPressed(key);
    }
}

void PixelAdventureScreen::keyReleased(uint8_t key) {
    this->level->keyReleased(key);
}

void PixelAdventureScreen::keyDown(uint8_t key) {
    this->level->keyDown(key);
}
