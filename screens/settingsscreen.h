#include "../core/common.h"
#include "../core/screen.h"
#include "../core/keyboard.h"
#include "../core/scene.h"

class SettingsScreen : public Screen
{
private:
    uint8_t option;
    uint16_t scroll;
    timetype lastUpdate;    
    TileMap *bgLayer;

    Scene3D *scene;
    Model3D model;
public:
    SettingsScreen(void (*returnCallBack)(int8_t menu, uint8_t option), void (*highScoreCallBack)(uint32_t highscore), uint32_t highscore, uint8_t option);
    ~SettingsScreen();

    void update(uint16_t deltaTimeMS);
    void draw(Display *display);
    void keyPressed(uint8_t key);
    void keyReleased(uint8_t key);
    void keyDown(uint8_t key);
};
