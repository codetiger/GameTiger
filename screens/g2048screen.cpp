#include "g2048screen.h"

/* Board move/combine code is copied from https://github.com/yerzhan7/2048/blob/master/2048/game.cpp */

G2048Screen::G2048Screen(void (*rcb)(int8_t menu), void (*hscb)(uint32_t highscore), uint32_t highscore) {
    printf("2048 screen loading...");
    this->screenId = 3;
    this->type = Type::GAME;
    this->returnCallBack = rcb;
    this->highScoreCallBack = hscb;
    this->highestValue = 2;
    this->score = 0;
    this->direction = -1;

    for (uint8_t i = 0; i < BOARDSIZE*BOARDSIZE; i++)
        board[i] = 0;
    this->addRandomBlock();
    this->addRandomBlock();
    printf("Done\n");
}

G2048Screen::~G2048Screen() {
}

void G2048Screen::addRandomBlock() {
    uint8_t x, y;
    for (uint8_t i = 0; i < BOARDSIZE*BOARDSIZE; i++) {
        x = rand() % BOARDSIZE;
        y = rand() % BOARDSIZE;
        if(board[y*BOARDSIZE + x] == 0)
            break;
    }
    board[y*BOARDSIZE + x] = 1;
}

void G2048Screen::move(bool& valid_step) {
    switch (this->direction) {
    case KEY_UP:
        for (int j = 0; j < BOARDSIZE; ++j) {
            for (int i = 0; i < BOARDSIZE; ++i) {
                if (board[i*BOARDSIZE + j] == 0) {
                    for (int k = i + 1; k < BOARDSIZE; ++k) {
                        if (board[k*BOARDSIZE + j] != 0) {
                            valid_step = true;
                            board[i*BOARDSIZE + j] = board[k*BOARDSIZE + j];
                            board[k*BOARDSIZE + j] = 0;
                            break;
                        }
                    }
                }
            }
        }

        break;
    case KEY_DOWN:
        for (int j = 0; j < BOARDSIZE; ++j) {
            for (int i = BOARDSIZE - 1; i >= 0; --i) {
                if (board[i*BOARDSIZE + j] == 0) { 
                    for (int k = i - 1; k >= 0; --k) { 
                        if (board[k*BOARDSIZE + j] != 0) { 
                            valid_step = true;
                            board[i*BOARDSIZE + j] = board[k*BOARDSIZE + j]; 
                            board[k*BOARDSIZE + j] = 0;
                            break; 
                        }
                    }
                }
            }
        }
        break;
    case KEY_LEFT:
        for (int i = 0; i < BOARDSIZE; ++i) {
            for (int j = 0; j < BOARDSIZE; ++j) {
                if (board[i*BOARDSIZE + j] == 0) {
                    for (int k = j + 1; k < BOARDSIZE; ++k) {
                        if (board[i*BOARDSIZE + k] != 0) {
                            valid_step = true;
                            board[i*BOARDSIZE + j] = board[i*BOARDSIZE + k];
                            board[i*BOARDSIZE + k] = 0;
                            break;
                        }
                    }
                }
            }
        }
        break;
    case KEY_RIGHT:
        for (int i = 0; i < BOARDSIZE; ++i) {
            for (int j = BOARDSIZE - 1; j >= 0; --j) {
                if (board[i*BOARDSIZE + j] == 0) {
                    for (int k = j - 1; k >= 0; --k) {
                        if (board[i*BOARDSIZE + k] != 0) {
                            valid_step = true;
                            board[i*BOARDSIZE + j] = board[i*BOARDSIZE + k];
                            board[i*BOARDSIZE + k] = 0;
                            break;
                        }
                    }
                }
            }
        }
        break;
    }
}

void G2048Screen::combine(bool& valid_step) {
    switch (this->direction) {
    case KEY_UP:
        for (int j = 0; j < BOARDSIZE; ++j) {
            for (int i = 0; i < BOARDSIZE - 1; ++i) {
                if ((board[i*BOARDSIZE + j] != 0) && (board[i*BOARDSIZE + j] == board[(i + 1)*BOARDSIZE + j])) {
                    valid_step = true;
                    board[i*BOARDSIZE + j]++; 
                    board[(i + 1)*BOARDSIZE + j] = 0;
                    score += board[i*BOARDSIZE + j];
                }
            }
        }
        break;
    case KEY_DOWN:
        for (int j = 0; j < BOARDSIZE; ++j) {
            for (int i = BOARDSIZE - 1; i >= 1; --i) {
                if ((board[i*BOARDSIZE + j] != 0) && (board[i*BOARDSIZE + j] == board[(i - 1)*BOARDSIZE + j])) {
                    valid_step = true;
                    board[i*BOARDSIZE + j]++;
                    board[(i - 1)*BOARDSIZE + j] = 0;
                    score += board[i*BOARDSIZE + j]*board[i*BOARDSIZE + j];
                }
            }
        }
        break;
    case KEY_LEFT:
        for (int i = 0; i < BOARDSIZE; ++i) {
            for (int j = 0; j < BOARDSIZE - 1; ++j) {
                if ((board[i*BOARDSIZE + j] != 0) && (board[i*BOARDSIZE + j] == board[i*BOARDSIZE + j + 1])) {
                    valid_step = true;
                    board[i*BOARDSIZE + j]++;
                    board[i*BOARDSIZE + j + 1] = 0;
                    score += board[i*BOARDSIZE + j];
                }
            }
        }
        break;
    case KEY_RIGHT:
        for (int i = 0; i < BOARDSIZE; ++i) {
            for (int j = BOARDSIZE - 1; j >= 1; --j) {
                if ((board[i*BOARDSIZE + j] != 0) && (board[i*BOARDSIZE + j] == board[i*BOARDSIZE + j - 1])) {
                    valid_step = true;
                    board[i*BOARDSIZE + j]++;
                    board[i*BOARDSIZE + j - 1] = 0;
                    score += board[i*BOARDSIZE + j];
                }
            }
        }
        break;
    }
}

void G2048Screen::update(uint16_t deltaTimeMS) {
    if(this->direction != -1) {
        bool valid_step = false;
        move(valid_step);
        combine(valid_step);
        if(valid_step) {
            move(valid_step);
            this->addRandomBlock();
        }
        this->direction = -1;
        // printBoard();
    }
}

void G2048Screen::printBoard() {
    for (uint8_t i = 0; i < BOARDSIZE; i++) {
        for (uint8_t j = 0; j < BOARDSIZE; j++) {
            printf("%d\t", board[i*BOARDSIZE+j]);
        }
        printf("\n");
    }
    printf("\n");
}

void G2048Screen::draw(Display *display) {
    display->clear(Color(156, 140, 125));
    for (uint8_t i = 0; i < BOARDSIZE; i++) {
        for (uint8_t j = 0; j < BOARDSIZE; j++) {
            Color c = board[i*BOARDSIZE+j] < 12 ? colors[board[i*BOARDSIZE+j]] : Color(32, 32, 32);
            display->fillRect(5 + 80*j, 5 + 60*i, 70, 50, c);
            if (board[i*BOARDSIZE+j]) {
                std::string str = std::to_string((uint16_t)pow(2, board[i*BOARDSIZE+j]));
                uint16_t width = font.getWidth(str);
                font.drawSprites(display, str, 5 + (70-width)/2 + 80*j, 22 + 60*i);
            }
        }
    }
}

void G2048Screen::keyPressed(uint8_t key) {
    if(key == KEY_DOWN || key == KEY_UP || key == KEY_LEFT || key == KEY_RIGHT)
        this->direction = key;
    else if(key == KEY_B)
        this->returnCallBack(-1);
}

void G2048Screen::keyReleased(uint8_t key) {
}

void G2048Screen::keyDown(uint8_t key) {
}
