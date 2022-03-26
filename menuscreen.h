#include "pico/stdlib.h"
#include "core/screen.h"
#include "core/image.h"
#include <string>

class MenuScreen : public Screen
{
private:
    Image *font;
    Image *menuItemLogo[5];
    const uint8_t menuCount = 5;
    const std::string menuItemNames[5] = {
        "SNAKE ", 
        " 2048 ",
        "TETRIS",
        " ONE  ",
        " TWO  "
    };
    uint8_t selectedMenuItem, currentMenuItem, animationCounter;
    bool isAnimating;
    const uint8_t menuItemGap = 160;
public:
    MenuScreen(void (*returnCallBack)(int8_t menu), void (*highScoreCallBack)(uint32_t highscore), uint32_t highscore);
    ~MenuScreen();

    void update();
    void draw(Display *display);
    void keyPressed(uint8_t key);
    void keyReleased(uint8_t key);
    void keyDown(uint8_t key);
};
