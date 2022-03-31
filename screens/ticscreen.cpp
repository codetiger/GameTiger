#include "ticscreen.h"

TicScreen::TicScreen(void (*rcb)(int8_t menu), void (*hscb)(uint32_t highscore), uint32_t highscore) {
    printf("Tic screen loading...");
    this->screenId = 6;
    this->type = Type::GAME;
    this->returnCallBack = rcb;
    this->highScoreCallBack = hscb;
    this->gameState = PLAYING;
    this->turn = ((rand() % 100) % 2) == 0 ? O_TIC : X_TIC;
    this->won = E_TIC;
    this->selectedX = this->selectedY = 0;

    for (uint8_t i = 0; i < TIC_BOARDSIZE*TIC_BOARDSIZE; i++)
        board[i] = E_TIC;

    font.setAlpha(255);
    this->totalDuration = 0;
    printf("Done\n");
}

TicScreen::~TicScreen() {

}

void TicScreen::update(uint16_t deltaTimeMS) {
    this->totalDuration += deltaTimeMS;
    if(turn == X_TIC && this->gameState == PLAYING && this->totalDuration > 1500) {
        uint8_t x, y;
        for (uint8_t i = 0; i < TIC_BOARDSIZE*TIC_BOARDSIZE; i++) {
            x = rand() % TIC_BOARDSIZE;
            y = rand() % TIC_BOARDSIZE;
            if(board[y*TIC_BOARDSIZE + x] == E_TIC) {
                board[y*TIC_BOARDSIZE + x] = X_TIC;
                turn = O_TIC;
                if(checkGameOver())
                    this->gameState = LOST;
                break;
            }
        }
        this->totalDuration = 0;
    }
}

void TicScreen::draw(Display *display) {
    display->clear(Color(100, 71, 195));
    for (uint8_t i = 0; i < TIC_BOARDSIZE; i++) {
        for (uint8_t j = 0; j < TIC_BOARDSIZE; j++) {
            if(board[i*TIC_BOARDSIZE+j] == E_TIC)
                tttblocks.drawSprite(display, 'e', 30+j*60, 30+i*60);
            else if(board[i*TIC_BOARDSIZE+j] == O_TIC)
                tttblocks.drawSprite(display, 'o', 30+j*60, 30+i*60);
            else if(board[i*TIC_BOARDSIZE+j] == X_TIC)
                tttblocks.drawSprite(display, 'x', 30+j*60, 30+i*60);
        }
    }

    font.drawSprites(display, "TIC TAC TOE", 90, 10);
    if(turn == O_TIC)
        display->rect(30+this->selectedX*60, 30+this->selectedY*60, 60, 60, Color(0, 255, 0));
    
    if(this->gameState == PLAYING) {
        font.drawSprites(display, "TURN", 240, 140);
        if(turn == O_TIC)
            font.drawSprites(display, "YOU", 242, 160);
        else
            font.drawSprites(display, "COMP", 240, 160);
    } else {
        std::string str = "";
        if(won == O_TIC)
            str = "YOU WON";
        else if(won == X_TIC)
            str = "COMPUTER WON";
        else
            str = "GAME DRAW";
        uint8_t width = font.getWidth(str);
        font.drawSprites(display, str, (240-width)/2, 215);
    }
}

bool TicScreen::checkGameOver() {
    if(board[0*TIC_BOARDSIZE+0] != E_TIC && 
        board[1*TIC_BOARDSIZE+1] == board[0*TIC_BOARDSIZE+0] &&
        board[2*TIC_BOARDSIZE+2] == board[1*TIC_BOARDSIZE+1]) {
        won = board[0];
        return true;
    }
    if(board[2*TIC_BOARDSIZE+0] != E_TIC && 
        board[1*TIC_BOARDSIZE+1] == board[2*TIC_BOARDSIZE+0] &&
        board[0*TIC_BOARDSIZE+2] == board[1*TIC_BOARDSIZE+1]) {
        won = board[1*TIC_BOARDSIZE+1];
        return true;
    }
    for(int i = 0; i < TIC_BOARDSIZE; i++) {
        if(board[i*TIC_BOARDSIZE+0] != E_TIC && 
            board[i*TIC_BOARDSIZE+0] == board[i*TIC_BOARDSIZE+1] && 
            board[i*TIC_BOARDSIZE+1] == board[i*TIC_BOARDSIZE+2]) {
            won = board[i*TIC_BOARDSIZE+0];
            return true;
        }
    }
    for(int j = 0; j < TIC_BOARDSIZE; j++) {
        if(board[j] != E_TIC && 
            board[j] == board[TIC_BOARDSIZE+j] && 
            board[TIC_BOARDSIZE+j] == board[2*TIC_BOARDSIZE+j]) {
            won = board[j];
            return true;
        }
    }

    for(int i = 0; i < TIC_BOARDSIZE*TIC_BOARDSIZE; i++)
        if(board[i] == E_TIC)
            return false;

    won = E_TIC;
    return true;
}

void TicScreen::printBoard() {
    for (uint8_t i = 0; i < TIC_BOARDSIZE; i++) {
        for (uint8_t j = 0; j < TIC_BOARDSIZE; j++) {
            printf("%d\t", board[i*TIC_BOARDSIZE+j]);
        }
        printf("\n");
    }
    printf("\n");
}

void TicScreen::keyPressed(uint8_t key) {
    if(key == KEY_DOWN) {
        this->selectedY = (this->selectedY < TIC_BOARDSIZE-1) ? this->selectedY+1 : this->selectedY;
    } else if(key == KEY_UP) {
        this->selectedY = (this->selectedY > 0) ? this->selectedY-1 : this->selectedY;
    } else if(key == KEY_RIGHT) {
        this->selectedX = (this->selectedX < TIC_BOARDSIZE-1) ? this->selectedX+1 : this->selectedX;
    } else if(key == KEY_LEFT) {
        this->selectedX = (this->selectedX > 0) ? this->selectedX-1 : this->selectedX;
    } else if(key == KEY_A) {
        if(this->gameState == PLAYING && turn == O_TIC && board[this->selectedY*TIC_BOARDSIZE+this->selectedX] == E_TIC) {
            board[this->selectedY*TIC_BOARDSIZE+this->selectedX] = O_TIC;
            turn = X_TIC;
            if(checkGameOver())
                this->gameState = LOST;
        }
    } else if(key == KEY_B) {
        if(this->gameState == LOST)
            this->returnCallBack(4);
    }
    // printBoard();
}

void TicScreen::keyReleased(uint8_t key) {

}

void TicScreen::keyDown(uint8_t key) {

}
