#include "pico/stdlib.h"
#include "snakescreen.h"
#include "content/font2.h"
#include "content/gameover.h"
#include <string> 

SnakeScreen::SnakeScreen(void (*rcb)(int8_t menu), void (*hscb)(uint32_t highscore), uint32_t hs) {
    this->screenId = 2;
    this->type = Type::GAME;
    this->highScore = hs;
    this->returnCallBack = rcb;
    this->highScoreCallBack = hscb;
    this->font2 = new Image(font2_img_width, font2_img_height, font2_color_count, (uint8_t*)font2_palette, (uint8_t*)font2_pixel_data, font2_sprite_data);
    this->gameOver = new Image(gameover_img_width, gameover_img_height, gameover_color_count, (uint8_t*)gameover_palette, (uint8_t*)gameover_pixel_data);

    this->snakeBlocks[0][0] = 15, this->snakeBlocks[0][1] = 12;
    this->snakeBlocks[1][0] = 14, this->snakeBlocks[1][1] = 12;
    this->snakeBlocks[2][0] = 13, this->snakeBlocks[2][1] = 12;

    this->gameState = WAITING;
    this->createNewFood();

    this->lastUpdate = to_ms_since_boot(get_absolute_time());
    this->gameSpeed = (this->snakeLength / 10) + 1;
}

SnakeScreen::~SnakeScreen() {
}

void SnakeScreen::createNewFood() {
    this->foodPos[0] = rand() % BOARD_WIDTH;
    this->foodPos[1] = rand() % BOARD_HEIGHT;
}

void SnakeScreen::moveSnake() {
    int8_t newPos[2] = {(int8_t)this->snakeBlocks[0][0] + this->DIR_INFO[this->snakeDir][0], (int8_t)this->snakeBlocks[0][1] + this->DIR_INFO[this->snakeDir][1]};
    if(newPos[0] < 0 || newPos[0] >= BOARD_WIDTH || newPos[1] < 0 || newPos[1] >= BOARD_HEIGHT || this->checkSnakeLoop()) {
        this->gameState = LOST;
        return;
    }

    for (int i = this->snakeLength-1; i > 0; i--) {
        this->snakeBlocks[i][0] = this->snakeBlocks[i-1][0];
        this->snakeBlocks[i][1] = this->snakeBlocks[i-1][1];
    }
    this->snakeBlocks[0][0] = newPos[0];
    this->snakeBlocks[0][1] = newPos[1];
    
    if(this->checkFoodCaptured()) {
        this->snakeBlocks[this->snakeLength][0] = this->snakeBlocks[this->snakeLength-1][0];
        this->snakeBlocks[this->snakeLength][1] = this->snakeBlocks[this->snakeLength-1][1];
        this->snakeLength++;
        this->createNewFood();
        this->gameSpeed = (this->snakeLength / 10) + 1;
        if(this->highScore < this->snakeLength - 3) {
            this->highScore = this->snakeLength - 3;
            this->highScoreCallBack(this->highScore);
        }
    }
}

void SnakeScreen::update() {
    uint16_t timeDiff = to_ms_since_boot(get_absolute_time()) - this->lastUpdate;
    if(this->gameState == PLAYING && timeDiff > 350 - this->gameSpeed*25) {
        this->lastUpdate = to_ms_since_boot(get_absolute_time());
        this->moveSnake();
    }
}

bool SnakeScreen::checkFoodCaptured() {
    for (int i = 0; i < this->snakeLength; i++)
        if(this->snakeBlocks[i][0] == this->foodPos[0] && this->snakeBlocks[i][1] == this->foodPos[1])
            return true;
    return false;
}

bool SnakeScreen::checkSnakeLoop() {
    for (int i = 1; i < this->snakeLength; i++)
        if(this->snakeBlocks[i][0] == this->snakeBlocks[0][0] && this->snakeBlocks[i][1] == this->snakeBlocks[0][1])
            return true;
    return false;
}

void SnakeScreen::drawBlock(Display *display, uint16_t x, uint16_t y) {
    display->rect(x, y, 10, 10, this->dark);
    display->fillRect(x+2, y+2, 7, 7, this->dark);
}

void SnakeScreen::draw(Display *display) {
    display->clear(light);
    display->rect(10, 30, BOARD_WIDTH*10, BOARD_HEIGHT*10, dark);
    for (int i = 0; i < this->snakeLength; i++)
        this->drawBlock(display, 10 + this->snakeBlocks[i][0]*10, 30 + this->snakeBlocks[i][1]*10);

    this->drawBlock(display, 10 + this->foodPos[0]*10, 30 + this->foodPos[1]*10);
    this->font2->drawSprites(display, std::to_string(this->snakeLength-3), 200, 8);
    this->font2->drawSprites(display, std::to_string(this->highScore), 12, 8);
    if(this->gameState == LOST)
        this->gameOver->draw(display, 96, 80);
}

void SnakeScreen::keyPressed(uint8_t key) {
    if((key == KEY_DOWN && this->snakeDir != KEY_UP) || 
        (key == KEY_UP && this->snakeDir != KEY_DOWN) || 
        (key == KEY_LEFT && this->snakeDir != KEY_RIGHT) || 
        (key == KEY_RIGHT && this->snakeDir != KEY_LEFT)) {
        this->snakeDir = key;
    } else if(key == KEY_A && this->gameState == WAITING)
        this->gameState = PLAYING;
    else if(key == KEY_B && this->gameState == LOST)
        this->returnCallBack(-1);
}

void SnakeScreen::keyReleased(uint8_t key) {
}

void SnakeScreen::keyDown(uint8_t key) {
}
