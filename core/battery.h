#include "common.h"
#include "display.h"

class Battery {
private:
    const float FULL_BATTERY = 4.25;
    const float EMPTY_BATTERY = 2.54;

    const uint8_t VSYS_PIN = 26;
    const uint8_t POWER_PIN = 16;

    uint8_t levelCached = 0;
    uint32_t lastCachedTime = 0;
public:
    Battery();
    ~Battery();
    
    uint8_t getLevel();
    bool isCharging();
    void drawLevel(Display* display);
};

