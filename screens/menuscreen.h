#include "../core/common.h"
#include "../core/screen.h"

class MenuScreen : public Screen
{
private:
    const uint8_t menuCount = 9;
    const std::string menuItemNames[9][4] = {
        {"Snake", "", "", ""},
        {"Game Boy", "", "", ""},
        {"2048", "", "", ""},
        {"Tetris", "Easy", "Medium", "Hard"},
        {"Minesweeper", "Easy", "Medium", "Hard"},
        {"Tic Tac Toe", "Easy", "Medium", "Hard"},
        {"Ninja Frog", "Level 01", "Level 02", "Level 03"},
        {"Settings", "", "", ""},
        {"About", "", "", ""}
    };
    uint8_t selectedMenuItem, currentMenuItem, animationCounter;
    uint8_t currentOptionItem;
    bool isAnimating = false;;
    const uint8_t menuItemGap = 160;
    TileMap *bgLayer;
    uint8_t option;

public:
    MenuScreen(void (*returnCallBack)(int8_t menu, uint8_t option), void (*highScoreCallBack)(uint32_t highscore), uint32_t highscore, uint8_t option);
    ~MenuScreen();

    void update(uint16_t deltaTimeMS);
    void draw(Display *display);
    void keyPressed(uint8_t key);
    void keyReleased(uint8_t key);
    void keyDown(uint8_t key);
};
