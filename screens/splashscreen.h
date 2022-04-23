#include "../core/common.h"
#include "../core/screen.h"

class SplashScreen : public Screen
{
private:
    int8_t tileMoveX = 0;
    int8_t tileMoveY = 0;
    uint8_t imageAlpha = 0;
    uint16_t totalDuration, accDeltaTimeMS;
    TileMap *bgLayer;
    uint8_t option;
    GameState state;
public:
    SplashScreen(void (*returnCallBack)(int8_t menu, uint8_t option), void (*highScoreCallBack)(uint32_t highscore), uint32_t highscore, uint8_t option);
    ~SplashScreen();

    void update(uint16_t deltaTimeMS);
    void draw(Display *display);
    void keyPressed(uint8_t key);
    void keyReleased(uint8_t key);
    void keyDown(uint8_t key);
};
