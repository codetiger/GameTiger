#include "../core/common.h"
#include "../core/screen.h"
#include "../core/keyboard.h"

class GameBoyScreen : public Screen
{
private:
    uint8_t option;
    timetype lastUpdate;

    void* gb_ptr;

public:
    GameBoyScreen(void (*returnCallBack)(int8_t menu, uint8_t option), void (*highScoreCallBack)(uint32_t highscore), uint32_t highscore, uint8_t option);
    ~GameBoyScreen();

	/* Pointer to allocated memory holding GB file. */
    uint8_t *rom;
	/* Pointer to allocated memory holding save file. */
	uint8_t *cart_ram;

    Display *display;
    
    Color *palette;

    void update(uint16_t deltaTimeMS);
    void draw(Display *display);
    void keyPressed(uint8_t key);
    void keyReleased(uint8_t key);
    void keyDown(uint8_t key);
};
