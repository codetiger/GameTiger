#include "pa2screen.h"
#include "pa2/pa2level.h"

PixelAdventureScreen::PixelAdventureScreen(void (*rcb)(int8_t menu, uint8_t option), void (*hscb)(uint32_t highscore), uint32_t highscore, uint8_t option) {
    printf("Pixel screen loading...");
    this->screenId = 5;
    this->type = Type::GAME;
    this->returnCallBack = rcb;
    this->highScoreCallBack = hscb;
    this->option = option;
    this->gameState = PLAYING;

    this->level = new Level();
    this->level->setBGLayer(&allGameSprite, bgFrames[rand() % 7], false);

    uint16_t *levelMap = new uint16_t[level01XCount*level01YCount];
    for (int i = 0; i < level01XCount*level01YCount; i++)
        levelMap[i] = terrainFrames[level01[i]-1];
    this->level->setGameLayer(&allGameSprite, level01XCount, level01YCount, levelMap, 24);
    for (int i = 0; i < level01FruitCount; i++) {
        uint16_t *anim = 0;
        switch (level01Fruits[i][2]){
        case 0:
            anim = (uint16_t*)AppleAnimSeq;break;
        case 1:
            anim = (uint16_t*)BananasAnimSeq;break;
        case 2:
            anim = (uint16_t*)CherriesAnimSeq;break;
        case 3:
            anim = (uint16_t*)KiwiAnimSeq;break;
        case 4:
            anim = (uint16_t*)MelonAnimSeq;break;
        case 5:
            anim = (uint16_t*)OrangeAnimSeq;break;
        case 6:
            anim = (uint16_t*)PineappleAnimSeq;break;
        case 7:
            anim = (uint16_t*)StrawberryAnimSeq;break;
        }
        GameItem item;
        item.state = IDLE;
        item.movementType = STATIC;
        item.x = level01Fruits[i][0];
        item.y = level01Fruits[i][1];
        item.speed = 2;
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

    for (int i = 0; i < level01EnemyCount; i++) {
        GameItem item;
        item.state = IDLE;
        item.movementType = HORIZONTAL;
        item.x = level01EnemyMushroom[i][0]; item.y = level01EnemyMushroom[i][1];
        item.minAxis = level01EnemyMushroom[i][2]; item.maxAxis = level01EnemyMushroom[i][3];
        item.width = allGameAlphaSprite.getSpriteWidth(mushroomIdleAnimSeq[0]);
        item.height = allGameAlphaSprite.getSpriteHeight(mushroomIdleAnimSeq[0]);
        item.sprite = &allGameAlphaSprite;
        item.deltaHealth = -100;
        item.deltaScore = 0;
        item.numIdleFrames = 14;
        item.idleSeq = (uint16_t*)mushroomIdleAnimSeq;
        item.numRunFrames = 16;
        item.runSeq = (uint16_t*)mushroomRunAnimSeq;
        item.numHitFrames = 5;
        item.hitSeq = (uint16_t*)mushroomHitAnimSeq;
        item.curFrameIndex = 0;
        this->level->addGameItem(item);
    }

    this->level->hero.x = 100;
    this->level->hero.y = 176;
    this->level->hero.width = allGameAlphaSprite.getSpriteWidth(heroIdleAnimSeq[0]);
    this->level->hero.height = allGameAlphaSprite.getSpriteHeight(heroIdleAnimSeq[0]);
    this->level->hero.sprite = &allGameAlphaSprite;
    this->level->hero.numStandingFrames = 11;
    this->level->hero.numHurtFrames = 7;
    this->level->hero.numWalkFrames = 12;
    this->level->hero.jumpFrame = heroJumpFrame;
    this->level->hero.fallFrame = heroFallFrame;
    this->level->hero.numDoubleJumpFrames = 6;
    this->level->hero.direction = false;
    this->level->hero.standingSeq = (uint16_t*)heroIdleAnimSeq;
    this->level->hero.hurtSeq = (uint16_t*)heroHitAnimSeq;
    this->level->hero.walkSeq = (uint16_t*)heroRunAnimSeq;
    this->level->hero.doubleJumpSeq = (uint16_t*)heroDoubleJumpAnimSeq;
    this->level->hero.curFrameIndex = 0;
    this->level->hero.state = STANDING;
    this->level->hero.direction = false;
    printf("Done\n");
}

PixelAdventureScreen::~PixelAdventureScreen() {
}

void PixelAdventureScreen::update(uint16_t deltaTimeMS) {
    this->level->update(deltaTimeMS);
}

void PixelAdventureScreen::draw(Display *display) {
    this->level->draw(display);

    if(this->gameState == PAUSED) {
        std::string str = "Game Paused";
        uint16_t width = alphanumfont.getWidth(str, 2);
        alphanumfont.drawSprites(display, str, (DISPLAY_WIDTH - width)/2, 108, 2);
        str = "Press A to continue";
        width = alphanumfont.getWidth(str, 1);
        alphanumfont.drawSprites(display, str, (DISPLAY_WIDTH - width)/2, 140, 1);
        str = "Press B to quit";
        width = alphanumfont.getWidth(str, 1);
        alphanumfont.drawSprites(display, str, (DISPLAY_WIDTH - width)/2, 160, 1);
    }
}

void PixelAdventureScreen::keyPressed(uint8_t key) {
    if(this->gameState == PAUSED) {
        if(key == KEY_B)
            this->returnCallBack(this->screenId, this->option);
        else if(key == KEY_A)
            this->gameState = PLAYING;
    } else if(this->gameState == PLAYING) {
        this->level->keyPressed(key);
        if(key == KEY_B)
            this->gameState = PAUSED;
    }
}

void PixelAdventureScreen::keyReleased(uint8_t key) {
    this->level->keyReleased(key);
}

void PixelAdventureScreen::keyDown(uint8_t key) {
    this->level->keyDown(key);
}
