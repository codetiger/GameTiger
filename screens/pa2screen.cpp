#include "pa2screen.h"
#include "pa2/pa2level.h"

PixelAdventureScreen::PixelAdventureScreen(void (*rcb)(int8_t menu, uint8_t option), void (*hscb)(uint32_t highscore), uint32_t highscore, uint8_t option) {
    printf("About screen loading...");
    this->screenId = 5;
    this->type = Type::GAME;
    this->returnCallBack = rcb;
    this->highScoreCallBack = hscb;
    this->option = option;

    // this->level = new Level();
    // this->level->setBGLayer(&pa2bg, rand() % 6, false);
    // this->level->setGameLayer(&pa2terrain, level01XCount, level01YCount, (uint8_t*)level01);
    // for (int i = 0; i < level01GoodiesCount; i++)
    //     this->level->addGoodie(level01Goodies[i][0], level01Goodies[i][1], &pa2fruits, level01Goodies[i][2], level01Goodies[i][3], 17, level01Goodies[i][4], 6, level01Goodies[i][5], fruitsAnimSeq);
    printf("Done\n");
}

PixelAdventureScreen::~PixelAdventureScreen() {
}

void PixelAdventureScreen::update(uint16_t deltaTimeMS) {
    this->level->update(deltaTimeMS);
}

void PixelAdventureScreen::draw(Display *display) {
    this->level->draw(display);
}

void PixelAdventureScreen::keyPressed(uint8_t key) {
    this->level->keyPressed(key);
    if(key == KEY_B)
        this->returnCallBack(this->screenId, this->option);
}

void PixelAdventureScreen::keyReleased(uint8_t key) {
    this->level->keyReleased(key);
}

void PixelAdventureScreen::keyDown(uint8_t key) {
    this->level->keyDown(key);
}
