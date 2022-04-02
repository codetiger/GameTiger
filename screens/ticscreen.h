#include "../core/common.h"
#include "../core/screen.h"
#include "../core/keyboard.h"

#define TIC_BOARDSIZE 3
enum TICTYPE {
    E_TIC = 0, 
    O_TIC = 1, 
    X_TIC = 2, 
};

struct TicBoard {
    union {
        uint32_t value;
        struct {
            uint8_t cell0:2;
            uint8_t cell1:2;
            uint8_t cell2:2;
            uint8_t cell3:2;
            uint8_t cell4:2;
            uint8_t cell5:2;
            uint8_t cell6:2;
            uint8_t cell7:2;
            uint8_t cell8:2;
        } Cells;
    };

    TICTYPE getCellValue(uint8_t x, uint8_t y) {
        uint8_t index = y * TIC_BOARDSIZE + x;
        return getCellValue(index);
    }

    TICTYPE getCellValue(uint8_t index) {
        if (index == 0) return (TICTYPE)Cells.cell0;
        else if(index == 1) return (TICTYPE)Cells.cell1;
        else if(index == 2) return (TICTYPE)Cells.cell2;
        else if(index == 3) return (TICTYPE)Cells.cell3;
        else if(index == 4) return (TICTYPE)Cells.cell4;
        else if(index == 5) return (TICTYPE)Cells.cell5;
        else if(index == 6) return (TICTYPE)Cells.cell6;
        else if(index == 7) return (TICTYPE)Cells.cell7;
        else return (TICTYPE)Cells.cell8;

    }

    void setCellValue(uint8_t x, uint8_t y, TICTYPE type) {
        uint8_t index = y * TIC_BOARDSIZE + x;
        setCellValue(index, type);
    }

    void setCellValue(uint8_t index, TICTYPE type) {
        if (index == 0) Cells.cell0 = (uint8_t)type;
        else if(index == 1) Cells.cell1 = (uint8_t)type;
        else if(index == 2) Cells.cell2 = (uint8_t)type;
        else if(index == 3) Cells.cell3 = (uint8_t)type;
        else if(index == 4) Cells.cell4 = (uint8_t)type;
        else if(index == 5) Cells.cell5 = (uint8_t)type;
        else if(index == 6) Cells.cell6 = (uint8_t)type;
        else if(index == 7) Cells.cell7 = (uint8_t)type;
        else Cells.cell8 = (uint8_t)type;
    }

    TICTYPE getWinner() {
        const uint8_t wins[8][3] = {{0,1,2}, {3,4,5}, {6,7,8}, {0,3,6}, {1,4,7}, {2,5,8}, {0,4,8}, {2,4,6}};
        for(int i = 0; i < 8; ++i) {
            if(getCellValue(wins[i][0]) != E_TIC &&
                getCellValue(wins[i][0]) == getCellValue(wins[i][1]) &&
                getCellValue(wins[i][0]) == getCellValue(wins[i][2]))
                  return getCellValue(wins[i][0]);
        }
        return E_TIC;
    }

    bool isFull() {
        for(int i = 0; i < TIC_BOARDSIZE*TIC_BOARDSIZE; i++)
            if(getCellValue(i) == E_TIC)
                return false;
        return true;
    }
};


class TicScreen : public Screen
{
private:
    TicBoard board;
    TICTYPE turn, won;
    GameState gameState;
    uint8_t sx, sy, moveCount;
    uint8_t pInd[9] = {0, 1, 2, 3, 4, 5, 6, 7, 8};

    void printBoard();
    bool checkGameOver();
    uint8_t bestMove(TicBoard b, uint8_t moveIndex);
    int16_t minimax(TicBoard b, uint16_t depth, bool isAI);
public:
    TicScreen(void (*returnCallBack)(int8_t menu), void (*highScoreCallBack)(uint32_t highscore), uint32_t highscore);
    ~TicScreen();

    void update(uint16_t deltaTimeMS);
    void draw(Display *display);
    void keyPressed(uint8_t key);
    void keyReleased(uint8_t key);
    void keyDown(uint8_t key);
};
