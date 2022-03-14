#include <stdio.h>
#include <stdlib.h>
#include "hardware/flash.h"
#include "hardware/sync.h"
#include "pico/stdlib.h"
#include <cstring>

#include "core/display.h"
#include "core/battery.h"
#include "core/keyboard.h"
#include "splashscreen.h"
#include "menuscreen.h"
#include "snakescreen.h"

#define HIGHSCORESIZE (FLASH_PAGE_SIZE/4)
#define FLASH_TARGET_OFFSET (1536 * 1024)

Screen *screen;
uint32_t highscores[HIGHSCORESIZE];

uint16_t printDuration(uint32_t *start) {
    uint16_t timeDiff = to_ms_since_boot(get_absolute_time()) - *start;
    // printf("FPS: %d Timetaken: %d\n", 1000 / timeDiff, timeDiff);
    *start = to_ms_since_boot(get_absolute_time());
    return timeDiff;
}

void highScoreHandler(uint32_t highscore) {
    highscores[0] = 64;highscores[1] = 128;
    highscores[screen->screenId] = highscore;
    uint32_t ints = save_and_disable_interrupts();
    flash_range_erase(FLASH_TARGET_OFFSET, FLASH_SECTOR_SIZE);
    flash_range_program(FLASH_TARGET_OFFSET, (uint8_t*)highscores, FLASH_PAGE_SIZE);
    restore_interrupts (ints);
}

void readHighScoreData() {
    const uint32_t* flash_target_contents = (const uint32_t *) (XIP_BASE + FLASH_TARGET_OFFSET);
    for (int i = 0; i < HIGHSCORESIZE; i++)
        highscores[i] = flash_target_contents[i];
    if(highscores[0] != 64 || highscores[1] != 128)
        for (int i = 0; i < HIGHSCORESIZE; i++)
            highscores[i] = 0;
}

void backHandler(int8_t menu) {
    if(screen->type == Type::SPLASH || screen->type == Type::GAME) {
        delete screen;
        screen = new MenuScreen(*backHandler, *highScoreHandler, 0);
    } else if(screen->type == Type::MENU) {
        delete screen;
        if(menu == 0)
            screen = new SnakeScreen(*backHandler, *highScoreHandler, highscores[2]);
    }
}

int main() {
    stdio_init_all();
    sleep_ms(3000);
    const uint LED_PIN = PICO_DEFAULT_LED_PIN;
    gpio_init(LED_PIN);
    gpio_set_dir(LED_PIN, GPIO_OUT);

    readHighScoreData(); 
    Display *display = new Display();
    display->clear(Color(255, 255, 255));

    screen = new SplashScreen(*backHandler, *highScoreHandler, 0);
    Battery *battery = new Battery();
    KeyBoard *keyboard = new KeyBoard();

    uint32_t start = to_ms_since_boot(get_absolute_time());
    while (true) {
        uint16_t duration = printDuration(&start);
        // gpio_xor_mask(1<<LED_PIN);
        screen->update();
        keyboard->checkKeyState(screen);

        screen->draw(display);
        battery->drawLevel(display);
        display->update();
    }
    return 0;
}