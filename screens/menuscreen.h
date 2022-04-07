#include "../core/common.h"
#include "../core/screen.h"

class MenuScreen : public Screen
{
private:
    const uint8_t menuCount = 7;
    const std::string menuItemNames[7] = {
        "SNAKE", 
        "2048",
        "TETRIS",
        "MINESWEEPER",
        "TIC TAC TOE",
        "SETTINGS",
        "ABOUT"
    };
    uint8_t selectedMenuItem, currentMenuItem, animationCounter;
    bool isAnimating = false;;
    const uint8_t menuItemGap = 160;
    TileMap *bgLayer;

public:
    MenuScreen(void (*returnCallBack)(int8_t menu), void (*highScoreCallBack)(uint32_t highscore), uint32_t highscore);
    ~MenuScreen();

    void update(uint16_t deltaTimeMS);
    void draw(Display *display);
    void keyPressed(uint8_t key);
    void keyReleased(uint8_t key);
    void keyDown(uint8_t key);
};
