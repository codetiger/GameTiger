#include "tetrisscreen.h"

TetrisScreen::TetrisScreen(void (*rcb)(int8_t menu, uint8_t option), void (*hscb)(uint32_t highscore), uint32_t hs, uint8_t option) {
    printf("Tetris screen loading...");
    this->screenId = 2;
    this->type = Type::GAME;
    this->highScore = hs;
    this->returnCallBack = rcb;
    this->highScoreCallBack = hscb;
    this->option = option;
    this->gameSpeed = this->option*4;

    for (int i = 0; i < BOARD_HEIGHT; i++)
        for (int j = 0; j < BOARD_HEIGHT; j++)
            this->board[i * BOARD_WIDTH + j] = 0;

    gameState = WAITING;

    this->lastUpdate = getTime();
    createNewBlock();
    createNewBlock();
    createNewBlock();
    printf("Done\n");
}

TetrisScreen::~TetrisScreen() {

}

void TetrisScreen::update(uint16_t deltaTimeMS) {
    uint16_t timeDiff = getTimeDiffMS(this->lastUpdate);
    if(this->gameState == PLAYING && timeDiff > 750 - this->gameSpeed*25) {
        this->lastUpdate = getTime();
        if(canMove(currentBlockX, currentBlockY+1, false))
            this->currentBlockY++;
        else {
            for (int r = 0; r < currentBlock.size; r++)
                for (int c = 0; c < currentBlock.size; c++)
                    if(board[(r + currentBlockY) * BOARD_WIDTH + (c + currentBlockX)] == 0 && currentBlock.cells[r * BLOCK_SIZE + c])
                        board[(r + currentBlockY) * BOARD_WIDTH + (c + currentBlockX)] = (uint8_t)currentBlock.type;
            
            checkColFull();
            if(isGameOver())
                gameState = LOST;
            else
                createNewBlock();
        }
    }
    // printBoard();
}

void TetrisScreen::draw(Display *display) {
    display->clear(Color(247, 72, 69));
    display->fillRect(45, 0, 5, 240, DARKBG);
    display->fillRect(194, 0, 5, 240, DARKBG);
    display->fillRect(50, 0, 144, 240, LIGHTBG);
    
    for (int r = 0; r < BOARD_HEIGHT; r++) {
        for (int c = 0; c < BOARD_WIDTH; c++) {
            uint8_t cellValue = board[r * BOARD_WIDTH + c];
            if(r >= currentBlockY && c >= currentBlockX && 
                r < currentBlockY + currentBlock.size && c < currentBlockX + currentBlock.size)
                if(cellValue == 0 && currentBlock.cells[(r-currentBlockY) * BLOCK_SIZE + (c-currentBlockX)])
                    cellValue = currentBlock.type;
            if(cellValue != 0) {
                display->fillRect(50+12*c, 12*r, 11, 11, blockColors[cellValue]);
                display->rect(50+12*c, 12*r, 12, 12, WHITECOLOR);
            }
        }
    }
    alphanumfont.drawText(display, std::to_string(this->score), 240, 200, 255, 2);

    display->fillRect(215, 25, 80, 80, DARKBG);
    display->fillRect(220, 30, 70, 70, LIGHTBG);

    for (int r = 0; r < BLOCK_SIZE; r++)
        for (int c = 0; c < BLOCK_SIZE; c++)
            if(nextBlock.cells[r * BLOCK_SIZE + c]) {
                display->fillRect(235+12*c, 45+12*r, 11, 11, blockColors[nextBlock.type]);
                display->rect(235+12*c, 45+12*r, 12, 12, WHITECOLOR);
            }

    if(this->gameState == LOST) {
        std::string str = "Game Over";
        uint16_t width = alphanumfont.getTextWidth(str, 2);
        alphanumfont.drawText(display, str, (DISPLAY_WIDTH - width)/2, 108, 255, 2);
    } else if(this->gameState == PAUSED) {
        std::string str = "Game Paused";
        uint16_t width = alphanumfont.getTextWidth(str, 2);
        alphanumfont.drawText(display, str, (DISPLAY_WIDTH - width)/2, 108, 255, 2);
        str = "Press A to continue";
        width = alphanumfont.getTextWidth(str);
        alphanumfont.drawText(display, str, (DISPLAY_WIDTH - width)/2, 140, 255, 1);
        str = "Press B to quit";
        width = alphanumfont.getTextWidth(str);
        alphanumfont.drawText(display, str, (DISPLAY_WIDTH - width)/2, 160, 255, 1);
    }
}

void TetrisScreen::printBoard() {
    for (int r = 0; r < BOARD_HEIGHT; r++) {
        for (int c = 0; c < BOARD_WIDTH; c++) {
            uint8_t cellValue = board[r * BOARD_WIDTH + c];
            if(r >= currentBlockY && c >= currentBlockX && 
                r < currentBlockY + currentBlock.size && c < currentBlockX + currentBlock.size)
                if(cellValue == 0 && currentBlock.cells[(r-currentBlockY) * BLOCK_SIZE + (c-currentBlockX)])
                    cellValue = currentBlock.type;
            printf("%d ", cellValue);
        }
        printf("\n");
    }
    printf("\n");
}

void TetrisScreen::createNewBlock() {
    currentBlockY = 0;
    currentBlockX = (BOARD_WIDTH / 2) - 2;
    BLOCKTYPE currentBlockType = static_cast<BLOCKTYPE>(1 + (rand() % totalBlockTypes));
    currentBlock = nextBlock;
    nextBlock = Block(currentBlockType);
}

bool TetrisScreen::canMove(int8_t newPosX, int8_t newPosY, bool rotate) {
    struct Block cb = currentBlock;
    if(rotate)
        cb.rotate();
    for (int r = 0; r < cb.size; r++) {
        for (int c = 0; c < cb.size; c++) {
            if(c + newPosX >= 0 && c + newPosX < BOARD_WIDTH && (rotate || r + newPosY < BOARD_HEIGHT)) {
                if(cb.cells[r * BLOCK_SIZE + c] && board[(r + newPosY) * BOARD_WIDTH + (c + newPosX)] > 0)
                    return false;
            } else {
                if(cb.cells[r * BLOCK_SIZE + c])
                    return false;
            }
        }
    }

    return true;
}

bool TetrisScreen::isGameOver() {
    for (int c = 0; c < BOARD_WIDTH; c++)
        if(board[c] != 0)
            return true;

    return false;
}

void TetrisScreen::checkColFull() {
    uint8_t rowsRemovedInSingleBlock = 0;
    for (int r = BOARD_HEIGHT - 1; r >= 0; r--) {
        bool isFull = true;
        for (int c = 0; c < BOARD_WIDTH; c++)
            isFull = isFull & (board[r * BOARD_WIDTH + c] > 0);

        if(isFull) {
            removeRow(r);
            r++;
            rowsRemovedInSingleBlock++;
        }
    }
    
    if(rowsRemovedInSingleBlock > 0)
        this->score += rowsRemovedInSingleBlock * rowsRemovedInSingleBlock;
}

void TetrisScreen::removeRow(uint8_t row) {
    for (int r = row - 1; r >= 0; r--)
        for (int c = 0; c < BOARD_WIDTH; c++)
            this->board[(r + 1) * BOARD_WIDTH + c] = this->board[r * BOARD_WIDTH + c];
}

void TetrisScreen::keyPressed(uint8_t key) {
    if(key == KEY_B && this->gameState == PLAYING)
        this->gameState = PAUSED;
    else if(key == KEY_B && this->gameState == PAUSED)
        this->returnCallBack(this->screenId, this->option);
    else if(key == KEY_A && (this->gameState == WAITING || this->gameState == PAUSED))
        this->gameState = PLAYING;
    else if(this->gameState == PLAYING) {
        if(key == KEY_LEFT && canMove(currentBlockX - 1, currentBlockY, false))
            currentBlockX--;
        else if(key == KEY_RIGHT && canMove(currentBlockX + 1, currentBlockY, false))
            currentBlockX++;
        else if(key == KEY_DOWN && canMove(currentBlockX, currentBlockY+1, false))
            currentBlockY++;
        else if(key == KEY_A && canMove(currentBlockX, currentBlockY, true))
            currentBlock.rotate();
        this->audioController->beep(1500);
    }
}

void TetrisScreen::keyReleased(uint8_t key) {
}

void TetrisScreen::keyDown(uint8_t key) {
    if(this->gameState == PLAYING && key == KEY_DOWN && canMove(currentBlockX, currentBlockY+1, false))
        currentBlockY++;
}