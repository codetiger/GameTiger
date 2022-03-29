#include "../core/common.h"
#include "../core/screen.h"
#include "../core/image.h"

class SplashScreen : public Screen
{
private:
    int8_t tileMoveX = 0;
    int8_t tileMoveY = 0;
    Image *tiger, *font;
    uint8_t imageAlpha = 0;
    timetype startTime;
public:
    SplashScreen(void (*returnCallBack)(int8_t menu), void (*highScoreCallBack)(uint32_t highscore), uint32_t highscore);
    ~SplashScreen();

    void update();
    void draw(Display *display);
    void keyPressed(uint8_t key);
    void keyReleased(uint8_t key);
    void keyDown(uint8_t key);
};
