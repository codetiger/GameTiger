#include "pico/stdlib.h"
#include "core/screen.h"
#include "core/image.h"

class SplashScreen : public Screen
{
private:
    int8_t tileMoveX = 0;
    int8_t tileMoveY = 0;
    Image *tiger, *font;
    uint8_t imageAlpha = 0;
    uint32_t startTime = 0;
public:
    SplashScreen(void (*returnCallBack)(int8_t menu));
    ~SplashScreen();

    void update();
    void draw(Display *display);
    void keyPressed(uint8_t key);
    void keyReleased(uint8_t key);
    void keyDown(uint8_t key);
};
