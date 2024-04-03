#include "minescreen.h"

MineScreen::MineScreen(void (*rcb)(int8_t menu, uint8_t option), void (*hscb)(uint32_t highscore), uint32_t highscore, uint8_t option) {
    printf("[MineScreen] loading...\n");
    this->screenId = ScreenEnum::MINESCREEN;
    this->returnCallBack = rcb;
    this->highScoreCallBack = hscb;

    this->selectedX = 0;
    this->selectedY = 0;
    this->gameState = PLAYING;
    this->option = option;
    this->TOTAL_MINES = 15 * this->option;

    resetBoard();
    // printBoard();
    printf("[MineScreen] Done\n");
}

MineScreen::~MineScreen() {

}

void MineScreen::update(uint16_t deltaTimeMS) {

}

void MineScreen::draw(Display *display) {
    for (int y = 0; y < MINE_BOARD_HEIGHT; y++) {
        for (int x = 0; x < MINE_BOARD_WIDTH; x++) {
            if(state[y*MINE_BOARD_WIDTH+x] == CLOSE)
                allGameSprite.drawSprite(display, sweeperFrames[9], Vec2(x, y)*16);
            else if(state[y*MINE_BOARD_WIDTH+x] == FLAG)
                allGameSprite.drawSprite(display, sweeperFrames[11], Vec2(x, y)*16);
            else if(state[y*MINE_BOARD_WIDTH+x] == DOUBT)
                allGameSprite.drawSprite(display, sweeperFrames[13], Vec2(x, y)*16);
            else if(state[y*MINE_BOARD_WIDTH+x] == OPEN) {
                if(board[y*MINE_BOARD_WIDTH+x] >= 0 && board[y*MINE_BOARD_WIDTH+x] <= 8)
                    allGameSprite.drawSprite(display, sweeperFrames[0]+board[y*MINE_BOARD_WIDTH+x], Vec2(x, y)*16);
                else if(board[y*MINE_BOARD_WIDTH+x] == 9)
                    allGameSprite.drawSprite(display, sweeperFrames[10], Vec2(x, y)*16);
            }
        }
    }

    if(this->gameState == LOST) {
        std::string str = "Game Over";
        uint16_t width = alphanumfont.getTextWidth(str, 2);
        alphanumfont.drawText(display, str, Vec2((DISPLAY_WIDTH - width)/2, 108), 255, 2);
    } else 
        display->rect(Rect2(selectedX*16, selectedY*16, 16, 16), REDCOLOR);
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
        if(board[y*MINE_BOARD_WIDTH+x] != 9) {
            board[y*MINE_BOARD_WIDTH+x] = 9;
            for (int dy = -1; dy <= 1; dy++) {
                for (int dx = -1; dx <= 1; dx++) {
                    if(dx != 0 || dy != 0) {
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
                if(board[selectedY*MINE_BOARD_WIDTH+selectedX] == 0) {
                    chainOpen(selectedX, selectedY);
                }

                if(board[selectedY*MINE_BOARD_WIDTH+selectedX] == 9) {
                    gameState = LOST;

                    for (int y = 0; y < MINE_BOARD_HEIGHT; y++)
                        for (int x = 0; x < MINE_BOARD_WIDTH; x++)
                            state[y*MINE_BOARD_WIDTH+x] = OPEN;
                }
            }
        }
    } else if(key == KEY_B) {
        if(state[selectedY*MINE_BOARD_WIDTH+selectedX] == CLOSE)
            state[selectedY*MINE_BOARD_WIDTH+selectedX] = FLAG;
        else if(state[selectedY*MINE_BOARD_WIDTH+selectedX] == FLAG)
            state[selectedY*MINE_BOARD_WIDTH+selectedX] = DOUBT;
        else if(state[selectedY*MINE_BOARD_WIDTH+selectedX] == DOUBT)
            state[selectedY*MINE_BOARD_WIDTH+selectedX] = CLOSE;
    } else if(key == KEY_SELECT) {
        this->returnCallBack(this->screenId, this->option);
    }
    // printBoard();
}

void MineScreen::keyReleased(uint8_t key) {

}

void MineScreen::keyDown(uint8_t key) {

}
