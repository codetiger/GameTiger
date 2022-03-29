#include "tetrisscreen.h"
#include "../content/font2.h"

TetrisScreen::TetrisScreen(void (*rcb)(int8_t menu), void (*hscb)(uint32_t highscore), uint32_t hs) {
    this->screenId = 4;
    this->type = Type::GAME;
    this->highScore = hs;
    this->returnCallBack = rcb;
    this->highScoreCallBack = hscb;
    this->gameSpeed = 1;

    for (int i = 0; i < BOARD_HEIGHT; i++)
        for (int j = 0; j < BOARD_HEIGHT; j++)
            this->board[i * BOARD_WIDTH + j] = 0;

    gameState = WAITING;
    this->font2 = new Image(font2_img_width, font2_img_height, font2_color_count, (uint8_t*)font2_palette, (uint8_t*)font2_pixel_data, font2_sprite_data);

    srand((unsigned int)time(0));
    this->lastUpdate = getTime();
    createNewBlock();
    createNewBlock();
    createNewBlock();
}

TetrisScreen::~TetrisScreen() {

}

void TetrisScreen::update() {
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
            createNewBlock();
        }
    }
    printBoard();
}

void TetrisScreen::draw(Display *display) {
    display->clear(Color(176, 160, 145));
    display->fillRect(50, 0, 144, 240, Color(0, 0, 0));
    
    for (int r = 0; r < BOARD_HEIGHT; r++) {
        for (int c = 0; c < BOARD_WIDTH; c++) {
            uint8_t cellValue = board[r * BOARD_WIDTH + c];
            if(r >= currentBlockY && c >= currentBlockX && 
                r < currentBlockY + currentBlock.size && c < currentBlockX + currentBlock.size)
                if(cellValue == 0 && currentBlock.cells[(r-currentBlockY) * BLOCK_SIZE + (c-currentBlockX)])
                    cellValue = currentBlock.type;
            if(cellValue != 0)
                display->fillRect(50+12*c, 12*r, 11, 11, blockColors[cellValue]);
        }
    }
    this->font2->drawSprites(display, std::to_string(this->score), 240, 220);

    display->fillRect(240, 40, 70, 70, Color(146, 130, 115));
    for (int r = 0; r < BLOCK_SIZE; r++)
        for (int c = 0; c < BLOCK_SIZE; c++)
            if(nextBlock.cells[r * BLOCK_SIZE + c])
                display->fillRect(250+12*c, 50+12*r, 11, 11, blockColors[nextBlock.type]);
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
    printf("New Block: %d\n", currentBlockType);
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
    if(this->gameState == WAITING) {
        this->gameState = PLAYING;
        return;
    }
    if(key == KEY_LEFT) {
        if (canMove(currentBlockX - 1, currentBlockY, false))
            currentBlockX--;
    } else if(key == KEY_DOWN) {
        if(canMove(currentBlockX, currentBlockY+1, false))
            this->currentBlockY++;
    } else if(key == KEY_RIGHT) {
        if (canMove(currentBlockX + 1, currentBlockY, false))
            currentBlockX++;
    } else if(key == KEY_A) {
        if(canMove(currentBlockX, currentBlockY, true))
            currentBlock.rotate();
    } else if(key == KEY_B)
        this->returnCallBack(-1);
}

void TetrisScreen::keyReleased(uint8_t key) {
}

void TetrisScreen::keyDown(uint8_t key) {
}
