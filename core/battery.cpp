#include "battery.h"
#include <stdio.h>
#include <math.h>
#include <algorithm>

Battery::Battery(/* args */) {
    printf("Battery driver loading...");
#if defined(FORMPU) && defined(ENABLE_BATTERY_MONITOR)
    this->lastCachedTime = to_ms_since_boot(get_absolute_time()) - 12000;
    // gpio_init(POWER_PIN);
    // gpio_set_dir(POWER_PIN, GPIO_IN);

    adc_init();
    adc_gpio_init(VSYS_PIN);
    adc_select_input(0);
#endif
    printf("Done\n");
}

uint8_t Battery::getLevel() {
#if defined(FORMPU) && defined(ENABLE_BATTERY_MONITOR)
    uint8_t timeDiffSec = (to_ms_since_boot(get_absolute_time()) - this->lastCachedTime) / CLOCKS_PER_SEC;
    if(timeDiffSec > 10) {
        adc_select_input(0);
        float voltage = adc_read() * 3 * 3.3f / (1 << 12);
        uint8_t percentage = 100 * ((voltage - EMPTY_BATTERY) / (FULL_BATTERY - EMPTY_BATTERY));
        this->levelCached = std::max(std::min((uint8_t)100, percentage), (uint8_t)0);
        this->lastCachedTime = to_ms_since_boot(get_absolute_time());
    }
#endif
    return this->levelCached;
}

bool Battery::isCharging() {
#if defined(FORMPU) && defined(ENABLE_BATTERY_MONITOR)
    // return gpio_get(POWER_PIN);
    return false;
#else
    return false;
#endif
}

void Battery::drawLevel(Display* display) {
#ifdef ENABLE_BATTERY_MONITOR
    Color c = Color(0, 0, 0);
    display->fillRect(Rect2(310, 11, 4, 6), c);
    display->rect(Rect2(280, 8, 30, 12), c);
    uint8_t level = this->getLevel();
    if(level < 20)
        c = Color(180, 0, 8);
    else if(level < 50)
        c = Color(220, 116, 8);
    else if(level > 80 && this->isCharging())
        c = Color(30, 195, 6);

    display->fillRect(Rect2(282, 10, 27 * this->getLevel() / 100, 9), c);
#endif
}

Battery::~Battery()
{
}
