#include "../core/common.h"
#include "../core/screen.h"
#include "../core/keyboard.h"

class PixelAdventureScreen : public Screen
{
private:
    uint8_t option;
    timetype lastUpdate;    
    TileMap *bgLayer, *gameLayer;
    uint8_t scroll;
    uint16_t gameScrollX, gameScrollY;

public:
    PixelAdventureScreen(void (*returnCallBack)(int8_t menu, uint8_t option), void (*highScoreCallBack)(uint32_t highscore), uint32_t highscore, uint8_t option);
    ~PixelAdventureScreen();

    void update(uint16_t deltaTimeMS);
    void draw(Display *display);
    void keyPressed(uint8_t key);
    void keyReleased(uint8_t key);
    void keyDown(uint8_t key);
};
