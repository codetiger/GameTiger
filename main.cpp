#include "core/common.h"
#include "core/display.h"
#include "core/battery.h"
#include "core/keyboard.h"
#include "screens/splashscreen.h"
#include "screens/menuscreen.h"
#include "screens/snakescreen.h"
#include "screens/g2048screen.h"
#include "screens/tetrisscreen.h"

#define HIGHSCORESIZE (FLASH_PAGE_SIZE/4)
#define FLASH_TARGET_OFFSET (1536 * 1024)

Screen *screen;
uint32_t highscores[HIGHSCORESIZE];

void highScoreHandler(uint32_t highscore) {
    highscores[0] = 64;highscores[1] = 128;
    highscores[screen->screenId] = highscore;
    #ifdef FORMPU
    uint32_t ints = save_and_disable_interrupts();
    flash_range_erase(FLASH_TARGET_OFFSET, FLASH_SECTOR_SIZE);
    flash_range_program(FLASH_TARGET_OFFSET, (uint8_t*)highscores, FLASH_PAGE_SIZE);
    restore_interrupts (ints);
    #endif
}

void readHighScoreData() {
    #ifdef FORMPU
    const uint32_t* flash_target_contents = (const uint32_t *) (XIP_BASE + FLASH_TARGET_OFFSET);
    for (int i = 0; i < HIGHSCORESIZE; i++)
        highscores[i] = flash_target_contents[i];
    if(highscores[0] != 64 || highscores[1] != 128)
        for (int i = 0; i < HIGHSCORESIZE; i++)
            highscores[i] = 0;
    #endif
}

void backHandler(int8_t menu) {
    if(screen->type == Type::SPLASH || screen->type == Type::GAME) {
        delete screen;
        screen = new MenuScreen(*backHandler, *highScoreHandler, 0);
    } else if(screen->type == Type::MENU) {
        delete screen;
        if(menu == 0)
            screen = new SnakeScreen(*backHandler, *highScoreHandler, highscores[2]);
        else if(menu == 1)
            screen = new G2048Screen(*backHandler, *highScoreHandler, highscores[3]);
        else if(menu == 2)
            screen = new TetrisScreen(*backHandler, *highScoreHandler, highscores[4]);
    }
}

int main(int argc, char *argv[]) {
    #ifdef FORMPU
    stdio_init_all();
    sleep_ms(3000);
    #endif

    readHighScoreData(); 
    Display *display = new Display();
    display->clear(Color(255, 255, 255));

    Battery *battery = new Battery();
    KeyBoard *keyboard = new KeyBoard();
    screen = new SplashScreen(*backHandler, *highScoreHandler, 0);

    timetype lastUpdate = getTime();
    bool close = false;
    while (!close) {
        uint16_t deltaTimeMS = getTimeDiffMS(lastUpdate);
        lastUpdate = getTime();

        screen->update(deltaTimeMS);
        keyboard->checkKeyState(screen);

        screen->draw(display);
        battery->drawLevel(display);
        display->update();
    }

    return EXIT_SUCCESS;
}