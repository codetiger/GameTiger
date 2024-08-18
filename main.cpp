#include "core/common.h"
#include "core/display.h"
#include "core/battery.h"
#include "core/keyboard.h"
#include "core/LoRa/lora.h"

#include "screens/splashscreen.h"
#include "screens/menuscreen.h"
#include "screens/snakescreen.h"
#include "screens/gameboyscreen.h"
#include "screens/g2048screen.h"
#include "screens/tetrisscreen.h"
#include "screens/minescreen.h"
#include "screens/ticscreen.h"
#include "screens/pa2screen.h"
#include "screens/aboutscreen.h"
#include "screens/settingsscreen.h"

#define HIGHSCORESIZE (FLASH_PAGE_SIZE/4)
#define FLASH_TARGET_OFFSET (1536 * 1024)

Screen *screen;
Lora *lora;
uint32_t highscores[64];
bool shouldSwitchScreen;
uint8_t newScreenId, newOption;

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

void backHandler(int8_t menu, uint8_t option) {
    newScreenId = menu;
    newOption = option;
    shouldSwitchScreen = true;
}

void checkScreenSwitch() {
    if(!shouldSwitchScreen)
        return;

    if(screen->screenId == ScreenEnum::MENUSCREEN) {
        delete screen;
        if(newScreenId == ScreenEnum::SNAKESCREEN)
            screen = new SnakeScreen(*backHandler, *highScoreHandler, highscores[newScreenId], newOption);
        else if(newScreenId == ScreenEnum::GAMEBOYSCREEN)
            screen = new GameBoyScreen(*backHandler, *highScoreHandler, highscores[newScreenId], newOption);
        else if(newScreenId == ScreenEnum::G2048SCREEN)
            screen = new G2048Screen(*backHandler, *highScoreHandler, highscores[newScreenId], newOption);
        else if(newScreenId == ScreenEnum::TETRISSCREEN)
            screen = new TetrisScreen(*backHandler, *highScoreHandler, highscores[newScreenId], newOption);
        else if(newScreenId == ScreenEnum::MINESCREEN)
            screen = new MineScreen(*backHandler, *highScoreHandler, highscores[newScreenId], newOption);
        else if(newScreenId == ScreenEnum::TICSCREEN)
            screen = new TicScreen(*backHandler, *highScoreHandler, highscores[newScreenId], newOption);
        else if(newScreenId == ScreenEnum::PA2SCREEN)
            screen = new PixelAdventureScreen(*backHandler, *highScoreHandler, highscores[newScreenId], newOption);
        else if(newScreenId == ScreenEnum::SETTINGSSCREEN)
            screen = new SettingsScreen(*backHandler, *highScoreHandler, highscores[newScreenId], newOption);
        else if(newScreenId == ScreenEnum::ABOUTSCREEN)
            screen = new AboutScreen(*backHandler, *highScoreHandler, highscores[newScreenId], newOption);
        else
            printf("[Main] Something failed badly\n");
    } else if(screen->screenId == ScreenEnum::SPLASHSCREEN) {
        delete screen;
        screen = new MenuScreen(*backHandler, *highScoreHandler, newScreenId, newOption);
    } else {
        delete screen;
        screen = new MenuScreen(*backHandler, *highScoreHandler, newScreenId-2, newOption);
    }
    shouldSwitchScreen = false;
}

int main(int argc, char *argv[]) {
    sleep_ms(50);
    vreg_set_voltage(VREG_VOLTAGE_1_20);
    sleep_ms(1);
    set_sys_clock_khz(266 * 1000, true);
    sleep_ms(50);

    stdio_init_all();
    sleep_ms(1000);

    printf("[Main] Starting\n");
    srand((unsigned int)time(0));
    readHighScoreData();
    Display *display = new Display();
    display->initDMAChannel();
    display->initSequence();
    display->clear(Color(255, 255, 255));
    display->update();

    Battery *battery = new Battery();
    KeyBoard *keyboard = new KeyBoard();
    screen = new SplashScreen(*backHandler, *highScoreHandler, 0, 1);

    timetype lastUpdate = getTime();
    bool close = false;
    while (!close) {
        uint16_t deltaTimeMS = getTimeDiffMS(lastUpdate);
        lastUpdate = getTime();

        screen->update(deltaTimeMS);
        keyboard->checkKeyState(screen);
        screen->draw(display);

        battery->drawLevel(display);
        // printf("[Main] FPS: %d\n", int(1000 / deltaTimeMS));
        display->update();
        checkScreenSwitch();
    }

    return EXIT_SUCCESS;
}