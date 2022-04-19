#include "pa2screen.h"
#include "pa2/pa2level.h"

PixelAdventureScreen::PixelAdventureScreen(void (*rcb)(int8_t menu, uint8_t option), void (*hscb)(uint32_t highscore), uint32_t highscore, uint8_t option) {
    printf("About screen loading...");
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
    this->level->setGameLayer(&allGameSprite, level01XCount, level01YCount, levelMap, 157);
    for (int i = 0; i < level01GoodiesCount; i++) {
        uint16_t *anim = 0;
        if(level01Goodies[i][4] == 0)
            anim = (uint16_t*)AppleAnimSeq;
        else if(level01Goodies[i][4] == 1)
            anim = (uint16_t*)BananasAnimSeq;
        else if(level01Goodies[i][4] == 2)
            anim = (uint16_t*)CherriesAnimSeq;
        else if(level01Goodies[i][4] == 3)
            anim = (uint16_t*)KiwiAnimSeq;
        else if(level01Goodies[i][4] == 4)
            anim = (uint16_t*)MelonAnimSeq;
        else if(level01Goodies[i][4] == 5)
            anim = (uint16_t*)OrangeAnimSeq;
        else if(level01Goodies[i][4] == 6)
            anim = (uint16_t*)PineappleAnimSeq;
        else if(level01Goodies[i][4] == 7)
            anim = (uint16_t*)StrawberryAnimSeq;
        this->level->addGoodie(level01Goodies[i][0], level01Goodies[i][1], &allGameSprite, level01Goodies[i][2], level01Goodies[i][3], 17, anim, 6, (uint16_t*)CollectedAnimSeq);
    }
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
