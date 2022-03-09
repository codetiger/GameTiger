#include <stdio.h>
#include <stdlib.h>

#include "pico/stdlib.h"
#include "core/display.h"
#include "core/battery.h"
#include "core/keyboard.h"
#include "splashscreen.h"
#include "menuscreen.h"
#include "snakescreen.h"

Screen *screen;

uint16_t printDuration(uint32_t *start) {
    uint16_t timeDiff = to_ms_since_boot(get_absolute_time()) - *start;
    printf("FPS: %d Timetaken: %d\n", 1000 / timeDiff, timeDiff);
    *start = to_ms_since_boot(get_absolute_time());
    return timeDiff;
}

void backHandler(int8_t menu) {
    if(screen->type == Type::SPLASH || screen->type == Type::GAME) {
        delete screen;
        screen = new MenuScreen(*backHandler);
    } else if(screen->type == Type::MENU) {
        delete screen;
        if(menu == 0)
            screen = new SnakeScreen(*backHandler);
    }
}

int main() {
    stdio_init_all();
    sleep_ms(3000);
    const uint LED_PIN = PICO_DEFAULT_LED_PIN;
    gpio_init(LED_PIN);
    gpio_set_dir(LED_PIN, GPIO_OUT);
 
    Display *display = new Display();
    display->clear(Color(255, 255, 255));

    screen = new SplashScreen(*backHandler);
    Battery *battery = new Battery();
    KeyBoard *keyboard = new KeyBoard();

    uint32_t start = to_ms_since_boot(get_absolute_time());
    while (true) {
        uint16_t duration = printDuration(&start);
        gpio_xor_mask(1<<LED_PIN);
        screen->update();
        keyboard->checkKeyState(screen);

        screen->draw(display);
        battery->drawLevel(display);
        display->update();
    }
    return 0;
}