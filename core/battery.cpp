#include "battery.h"
#include <stdio.h>
#include <math.h>
#include <algorithm>

Battery::Battery(/* args */) {
#ifdef FORMPU
    this->lastCachedTime = to_ms_since_boot(get_absolute_time()) - 12000;
    gpio_init(POWER_PIN);
    gpio_set_dir(POWER_PIN, GPIO_IN);

    // gpio_get(POWER_PIN, 1);
    adc_init();
    adc_gpio_init(VSYS_PIN);
    adc_select_input(3);
#endif
}

uint8_t Battery::getLevel() {
#ifdef FORMPU
    uint8_t timeDiffSec = (to_ms_since_boot(get_absolute_time()) - this->lastCachedTime) / CLOCKS_PER_SEC;
    if(timeDiffSec > 10) {
        adc_select_input(3);
        float voltage = adc_read() * 3 * 3.3f / (1 << 12);
        uint8_t percentage = 100 * ((voltage - EMPTY_BATTERY) / (FULL_BATTERY - EMPTY_BATTERY));
        this->levelCached = std::max(std::min((uint8_t)100, percentage), (uint8_t)0);
        this->lastCachedTime = to_ms_since_boot(get_absolute_time());
    }
#endif
    return this->levelCached;
}

bool Battery::isCharging() {
#ifdef FORMPU
    return gpio_get(POWER_PIN);
#else
    return false;
#endif
}

void Battery::drawLevel(Display* display) {
    Color c = Color(0, 0, 0);
    display->fillRect(310, 11, 4, 6, c);
    display->rect(280, 8, 30, 12, c);
    uint8_t level = this->getLevel();
    if(level < 20)
        c = Color(180, 0, 8);
    else if(level < 50)
        c = Color(220, 116, 8);
    else if(level > 80 && this->isCharging())
        c = Color(30, 195, 6);

    display->fillRect(282, 10, 27 * this->getLevel() / 100, 9, c);
}

Battery::~Battery()
{
}
