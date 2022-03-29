#include "minescreen.h"

MineScreen::MineScreen(void (*rcb)(int8_t menu), void (*hscb)(uint32_t highscore), uint32_t highscore) {
    this->screenId = 5;
    this->type = Type::GAME;
    this->returnCallBack = rcb;
    this->highScoreCallBack = hscb;

    this->selectedX = 0;
    this->selectedY = 0;
    this->sweeper = new Image(sweeper_img_width, sweeper_img_height, sweeper_color_count, (uint8_t*)sweeper_palette, (uint8_t*)sweeper_pixel_data, sweeper_sprite_data);
    this->gameOver = new Image(gameover_img_width, gameover_img_height, gameover_color_count, (uint8_t*)gameover_palette, (uint8_t*)gameover_pixel_data);
    this->gameState = PLAYING;

    resetBoard();
    printBoard();
}

MineScreen::~MineScreen() {

}

void MineScreen::update(uint16_t deltaTimeMS) {

}

void MineScreen::draw(Display *display) {
    for (int y = 0; y < MINE_BOARD_HEIGHT; y++) {
        for (int x = 0; x < MINE_BOARD_WIDTH; x++) {
            if(state[y*MINE_BOARD_WIDTH+x] == CLOSE)
                this->sweeper->drawSprite(display, 'a', x*16, y*16);
            else if(state[y*MINE_BOARD_WIDTH+x] == FLAG)
                this->sweeper->drawSprite(display, 'c', x*16, y*16);
            else if(state[y*MINE_BOARD_WIDTH+x] == DOUBT)
                this->sweeper->drawSprite(display, 'e', x*16, y*16);
            else if(state[y*MINE_BOARD_WIDTH+x] == OPEN) {
                if(board[y*MINE_BOARD_WIDTH+x] >= 0 && board[y*MINE_BOARD_WIDTH+x] <= 8)
                    this->sweeper->drawSprite(display, '0'+board[y*MINE_BOARD_WIDTH+x], x*16, y*16);
                else if(board[y*MINE_BOARD_WIDTH+x] == 9)
                    this->sweeper->drawSprite(display, 'b', x*16, y*16);
            }
        }
    }

    if(this->gameState == LOST)
        this->gameOver->draw(display, 96, 80);
    else 
        display->rect(selectedX*16, selectedY*16, 16, 16, Color(255, 0, 0));
}

void MineScreen::resetBoard() {
    for (int y = 0; y < MINE_BOARD_HEIGHT; y++) {
        for (int x = 0; x < MINE_BOARD_WIDTH; x++) {
            board[y*MINE_BOARD_WIDTH+x] = 0;
            state[y*MINE_BOARD_WIDTH+x] = CLOSE;
        }
    }

    for (int i = 0; i < TOTAL_MINES; i++) {
        uint8_t x = rand() % MINE_BOARD_WIDTH;
        uint8_t y = rand() % MINE_BOARD_HEIGHT;
        for (int dy = -1; dy <= 1; dy++) {
            for (int dx = -1; dx <= 1; dx++) {
                if(dx == 0 && dy == 0)
                    board[(y+dy)*MINE_BOARD_WIDTH+(x+dx)] = 9;
                else {
                    if(x+dx >= 0 && y+dy >= 0 && 
                        x+dx < MINE_BOARD_WIDTH && y+dy < MINE_BOARD_HEIGHT && 
                        board[(y+dy)*MINE_BOARD_WIDTH+(x+dx)] != 9) {
                            board[(y+dy)*MINE_BOARD_WIDTH+(x+dx)]++;
                        }
                }
            }
        }
    }
}

void MineScreen::printBoard() {
    for (int y = 0; y < MINE_BOARD_HEIGHT; y++) {
        for (int x = 0; x < MINE_BOARD_WIDTH; x++) {
            printf("%d ", board[y*MINE_BOARD_WIDTH+x]);
        }
        printf("\n");
    }
    printf("\n");
}

void MineScreen::chainOpen(uint8_t x, uint8_t y) {
    for (int dy = -1; dy <= 1; dy++) {
        for (int dx = -1; dx <= 1; dx++) {
            if(dx == 0 && dy == 0) {
            } else {
                if(x+dx >= 0 && y+dy >= 0 && 
                    x+dx < MINE_BOARD_WIDTH && y+dy < MINE_BOARD_HEIGHT) {
                        if(state[(y+dy)*MINE_BOARD_WIDTH+(x+dx)] == CLOSE) {
                            state[(y+dy)*MINE_BOARD_WIDTH+(x+dx)] = OPEN;
                            if(board[(y+dy)*MINE_BOARD_WIDTH+(x+dx)] == 0)
                                chainOpen(x+dx, y+dy);
                        }
                    }
            }
        }
    }
}

void MineScreen::keyPressed(uint8_t key) {
    if(key == KEY_DOWN) {
        this->selectedY = (this->selectedY < MINE_BOARD_HEIGHT-1) ? this->selectedY+1 : this->selectedY;
    } else if(key == KEY_UP) {
        this->selectedY = (this->selectedY > 0) ? this->selectedY-1 : this->selectedY;
    } else if(key == KEY_RIGHT) {
        this->selectedX = (this->selectedX < MINE_BOARD_WIDTH-1) ? this->selectedX+1 : this->selectedX;
    } else if(key == KEY_LEFT) {
        this->selectedX = (this->selectedX > 0) ? this->selectedX-1 : this->selectedX;
    } else if(key == KEY_A) {
        if(this->gameState == PLAYING) {
            if(state[selectedY*MINE_BOARD_WIDTH+selectedX] == CLOSE 
                || state[selectedY*MINE_BOARD_WIDTH+selectedX] == DOUBT) {
                state[selectedY*MINE_BOARD_WIDTH+selectedX] = OPEN;
                if(board[selectedY*MINE_BOARD_WIDTH+selectedX] == 0)
                    chainOpen(selectedX, selectedY);

                if(board[selectedY*MINE_BOARD_WIDTH+selectedX] == 9) {
                    gameState = LOST;
                    for (int y = 0; y < MINE_BOARD_HEIGHT; y++)
                        for (int x = 0; x < MINE_BOARD_WIDTH; x++)
                            state[y*MINE_BOARD_WIDTH+x] = OPEN;
                }
            }
        }
    } else if(key == KEY_B) {
        if(this->gameState == LOST) 
            this->returnCallBack(-1);
        else if(this->gameState == PLAYING) {
            if(state[selectedY*MINE_BOARD_WIDTH+selectedX] == CLOSE)
                state[selectedY*MINE_BOARD_WIDTH+selectedX] = FLAG;
            else if(state[selectedY*MINE_BOARD_WIDTH+selectedX] == FLAG)
                state[selectedY*MINE_BOARD_WIDTH+selectedX] = DOUBT;
            else if(state[selectedY*MINE_BOARD_WIDTH+selectedX] == DOUBT)
                state[selectedY*MINE_BOARD_WIDTH+selectedX] = CLOSE;
        }
    }
    printBoard();
}

void MineScreen::keyReleased(uint8_t key) {

}

void MineScreen::keyDown(uint8_t key) {

}