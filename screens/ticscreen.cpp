#include "ticscreen.h"
#include <algorithm> 

TicScreen::TicScreen(void (*rcb)(int8_t menu, uint8_t option), void (*hscb)(uint32_t highscore), uint32_t highscore, uint8_t option) {
    printf("Tic screen loading...");
    this->screenId = 4;
    this->type = Type::GAME;
    this->returnCallBack = rcb;
    this->highScoreCallBack = hscb;
    this->gameState = PLAYING;
    this->turn = ((rand() % 100) % 2) == 0 ? O_TIC : X_TIC;
    this->won = E_TIC;
    this->moveCount = 0;
    this->sx = this->sy = 0;
    this->option = option;
    if(this->option == 3)
        this->maxDepth = 9;
    else if(this->option == 2)
        this->maxDepth = 5;
    else if(this->option == 1)
        this->maxDepth = 2;

    std::random_shuffle(&pInd[0], &pInd[8]);

    for (uint8_t y = 0; y < TIC_BOARDSIZE; y++)
        for (uint8_t x = 0; x < TIC_BOARDSIZE; x++)
            board.setCellValue(x, y, E_TIC);
    // printBoard();
    printf("Done\n");
}

TicScreen::~TicScreen() {

}

void TicScreen::update(uint16_t deltaTimeMS) {
    if(turn == X_TIC && this->gameState == PLAYING) {
        board.setCellValue(bestMove(board, this->moveCount), X_TIC);
        this->moveCount++;
        turn = O_TIC;
        if(checkGameOver())
            this->gameState = LOST;
    }
}

void TicScreen::draw(Display *display) {
    display->clear(Color(100, 71, 195));
    for (uint8_t y = 0; y < TIC_BOARDSIZE; y++) {
        for (uint8_t x = 0; x < TIC_BOARDSIZE; x++) {
            if(board.getCellValue(x, y) == E_TIC)
                allGameSprite.drawSprite(display, tttBlockFrames[0], 30+x*60, 30+y*60);
            else if(board.getCellValue(x, y) == O_TIC)
                allGameSprite.drawSprite(display, tttBlockFrames[1], 30+x*60, 30+y*60);
            else if(board.getCellValue(x, y) == X_TIC)
                allGameSprite.drawSprite(display, tttBlockFrames[2], 30+x*60, 30+y*60);
        }
    }

    uint8_t width = alphanumfont.getWidth("Tic Tac Toe", 2);
    alphanumfont.drawSprites(display, "Tic Tac Toe", (DISPLAY_WIDTH-width)/2, 2, 2);
    if(turn == O_TIC)
        display->rect(30+this->sx*60, 30+this->sy*60, 60, 60, Color(0, 255, 0));
    
    if(this->gameState == PLAYING) {
        alphanumfont.drawSprites(display, "Turn", 236, 130, 2);
        if(turn == O_TIC)
            alphanumfont.drawSprites(display, "You", 240, 160, 2);
        else
            alphanumfont.drawSprites(display, "Comp", 238, 160, 2);
    } else {
        std::string str = "";
        if(won == O_TIC)
            str = "YOU WON";
        else if(won == X_TIC)
            str = "COMPUTER WON";
        else
            str = "GAME DRAW";
        uint8_t width = alphanumfont.getWidth(str, 2);
        alphanumfont.drawSprites(display, str, (DISPLAY_WIDTH-width)/2, 210, 2);
    }
}

bool TicScreen::checkGameOver() {
    won = board.getWinner();
    if(won != E_TIC)
        return true;

    if(!board.isFull())
        return false;

    won = E_TIC;
    return true;
}

int16_t TicScreen::minimax(TicBoard b, uint8_t origDepth, uint8_t curDepth, bool isAI) {
	int16_t score = 0, bestScore = isAI ? -999 : 999;
	if (board.getWinner() != E_TIC)
        return isAI ? -1 : 1;
	else if(curDepth < 9 && curDepth - origDepth < this->maxDepth) {
        for(int i = 0; i < TIC_BOARDSIZE*TIC_BOARDSIZE; i++) {
            if (board.getCellValue(pInd[i]) == E_TIC) {
                board.setCellValue(pInd[i], isAI ? X_TIC : O_TIC);
                score = minimax(board, origDepth, curDepth + 1, !isAI);
                board.setCellValue(pInd[i], E_TIC);
                if((isAI && score > bestScore) || (!isAI && score < bestScore))
                    bestScore = score;
            }
        }
        return bestScore;
    } else
        return 0;
}

uint8_t TicScreen::bestMove(TicBoard b, uint8_t moveCount) {
	uint8_t index = 0;
	int16_t score = 0, bestScore = -999;
    for(int i = 0; i < TIC_BOARDSIZE*TIC_BOARDSIZE; i++) {
        if (board.getCellValue(pInd[i]) == E_TIC) {
            board.setCellValue(pInd[i], X_TIC);
            score = minimax(board, moveCount, moveCount+1, false);
            board.setCellValue(pInd[i], E_TIC);
            if(score > bestScore) {
                bestScore = score;
                index = pInd[i];
            }
        }
    }
    return index;
}

void TicScreen::printBoard() {
    for (uint8_t y = 0; y < TIC_BOARDSIZE; y++) {
        for (uint8_t x = 0; x < TIC_BOARDSIZE; x++) {
            printf("%d\t", board.getCellValue(x, y));
        }
        printf("\n");
    }
    printf("Value %d\n", board.value);
}

void TicScreen::keyPressed(uint8_t key) {
    if(key == KEY_DOWN) {
        this->sy = (this->sy < TIC_BOARDSIZE-1) ? this->sy+1 : this->sy;
    } else if(key == KEY_UP) {
        this->sy = (this->sy > 0) ? this->sy-1 : this->sy;
    } else if(key == KEY_RIGHT) {
        this->sx = (this->sx < TIC_BOARDSIZE-1) ? this->sx+1 : this->sx;
    } else if(key == KEY_LEFT) {
        this->sx = (this->sx > 0) ? this->sx-1 : this->sx;
    } else if(key == KEY_A) {
        if(this->gameState == PLAYING && turn == O_TIC && board.getCellValue(this->sx, this->sy) == E_TIC) {
            board.setCellValue(this->sx, this->sy, O_TIC);
            turn = X_TIC;
            this->moveCount++;
            if(checkGameOver())
                this->gameState = LOST;
        }
    } else if(key == KEY_B) {
        if(this->gameState == LOST)
            this->returnCallBack(this->screenId, this->option);
    }
    // printBoard();
}

void TicScreen::keyReleased(uint8_t key) {

}

void TicScreen::keyDown(uint8_t key) {

}
