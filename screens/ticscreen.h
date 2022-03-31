#include "../core/common.h"
#include "../core/screen.h"
#include "../core/keyboard.h"

#define TIC_BOARDSIZE 3
enum TICTYPE {
    E_TIC = 0, 
    O_TIC = 1, 
    X_TIC = 2, 
};
enum PLAYERTYPE {
    COMPUTER_PLAYER = 0, 
    HUMAN_PLAYER = 1, 
};

class TicScreen : public Screen
{
private:
    TICTYPE board[TIC_BOARDSIZE*TIC_BOARDSIZE];
    TICTYPE turn, won;
    GameState gameState;
    uint8_t selectedX, selectedY;
    uint16_t totalDuration;

    void printBoard();
    bool checkGameOver();
public:
    TicScreen(void (*returnCallBack)(int8_t menu), void (*highScoreCallBack)(uint32_t highscore), uint32_t highscore);
    ~TicScreen();

    void update(uint16_t deltaTimeMS);
    void draw(Display *display);
    void keyPressed(uint8_t key);
    void keyReleased(uint8_t key);
    void keyDown(uint8_t key);
};
