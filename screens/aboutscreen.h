#include "../core/common.h"
#include "../core/screen.h"
#include "../core/keyboard.h"

class AboutScreen : public Screen
{
private:
    uint8_t option;
    uint16_t scroll;
    timetype lastUpdate;    
    TileMap *bgLayer;

    std::string aboutText[10][5] = {
        {"GAMETIGER", "Retro style gameconsole build using RPi Pico"},
        {"DESIGN", "Harishankar Narayanan"},
        {"PROGRAMMER", "Harishankar Narayanan"},
        {"QUALITY", "Mithila Muralidharan", "Naethraa H M", "Prajan H M"},
        {"HARDWARE", "Raspberry Pi Pico", "Waveshare 2 inch LCD display module", "Pimoroni LiPo SHIM for Pico", "Witty Fox LiIon Battery"},
        {"THANK YOU", "Remember that you are not alone", "There are others going through the same thing", "By Adam Levine"},
    };

public:
    AboutScreen(void (*returnCallBack)(int8_t menu, uint8_t option), void (*highScoreCallBack)(uint32_t highscore), uint32_t highscore, uint8_t option);
    ~AboutScreen();

    void update(uint16_t deltaTimeMS);
    void draw(Display *display);
    void keyPressed(uint8_t key);
    void keyReleased(uint8_t key);
    void keyDown(uint8_t key);
};
