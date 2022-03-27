#include "snakescreen.h"
#include "../content/font2.h"
#include "../content/gameover.h"

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

    this->lastUpdate = TimeSinceBoot;
    this->gameSpeed = 1;
}

SnakeScreen::~SnakeScreen() {
}

void SnakeScreen::createNewFood() {
    this->foodPos[0] = rand() % BOARD_WIDTH;
    this->foodPos[1] = rand() % BOARD_HEIGHT;
}

void SnakeScreen::moveSnake() {
    int8_t newPos[2] = {static_cast<int8_t>(this->snakeBlocks[0][0]), static_cast<int8_t>(this->snakeBlocks[0][1])};
    newPos[0] += this->DIR_INFO[this->snakeDir][0];
    newPos[1] += this->DIR_INFO[this->snakeDir][1];
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
        if(this->highScore < this->snakeLength - 3) {
            this->highScore = this->snakeLength - 3;
            this->highScoreCallBack(this->highScore);
        }
    }
}

void SnakeScreen::update() {
    uint16_t timeDiff = TimeSinceBoot - this->lastUpdate;
    if(this->gameState == PLAYING && timeDiff > 350 - this->gameSpeed*25) {
        this->lastUpdate = TimeSinceBoot;
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
    } else if(key == KEY_A) {
        if(this->gameState == WAITING)
            this->gameState = PLAYING;
        else if(this->gameState == PLAYING) 
            this->gameSpeed = (this->gameSpeed < 12) ? this->gameSpeed+1 : this->gameSpeed;
    } else if(key == KEY_B) {
        if(this->gameState == LOST) 
            this->returnCallBack(-1);
        else if(this->gameState == PLAYING)
            this->gameSpeed = (this->gameSpeed > 1) ? this->gameSpeed-1 : this->gameSpeed;
    }
}

void SnakeScreen::keyReleased(uint8_t key) {
}

void SnakeScreen::keyDown(uint8_t key) {
}
