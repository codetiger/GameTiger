#include "../core/common.h"
#include "../core/screen.h"
#include "../core/keyboard.h"

class PixelAdventureScreen : public Screen
{
private:
    GameState gameState;
    uint8_t option;
    Level *level;

public:
    PixelAdventureScreen(void (*returnCallBack)(int8_t menu, uint8_t option), void (*highScoreCallBack)(uint32_t highscore), uint32_t highscore, uint8_t option);
    ~PixelAdventureScreen();

    void update(uint16_t deltaTimeMS);
    void draw(Display *display);
    void keyPressed(uint8_t key);
    void keyReleased(uint8_t key);
    void keyDown(uint8_t key);
};
