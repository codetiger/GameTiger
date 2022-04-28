#include "core/common.h"
#include "core/display.h"
#include "core/battery.h"
#include "core/keyboard.h"
#include "core/audio.h"
#include "screens/splashscreen.h"
#include "screens/menuscreen.h"
#include "screens/snakescreen.h"
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
uint32_t highscores[64];
AudioController *audioController = new AudioController();

void audioControllerCore() {
    while (true) {
        int32_t num = multicore_fifo_pop_blocking();
        if(num == AUDIO_FLAG_VALUE)
            audioController->play();
    }
}

void highScoreHandler(uint32_t highscore) {
    highscores[0] = 64;highscores[1] = 128;
    highscores[screen->screenId+2] = highscore;
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

void backHandler(int8_t menu, uint8_t option) {
    if(screen->type == Type::SPLASH || screen->type == Type::GAME) {
        delete screen;
        screen = new MenuScreen(*backHandler, *highScoreHandler, menu, option);
    } else if(screen->type == Type::MENU) {
        delete screen;
        if(menu == 0)
            screen = new SnakeScreen(*backHandler, *highScoreHandler, highscores[2], option);
        else if(menu == 1)
            screen = new G2048Screen(*backHandler, *highScoreHandler, highscores[3], option);
        else if(menu == 2)
            screen = new TetrisScreen(*backHandler, *highScoreHandler, highscores[4], option);
        else if(menu == 3)
            screen = new MineScreen(*backHandler, *highScoreHandler, highscores[5], option);
        else if(menu == 4)
            screen = new TicScreen(*backHandler, *highScoreHandler, highscores[6], option);
        else if(menu == 5)
            screen = new PixelAdventureScreen(*backHandler, *highScoreHandler, highscores[7], option);
        else if(menu == 6)
            screen = new SettingsScreen(*backHandler, *highScoreHandler, highscores[8], option);
        else if(menu == 7)
            screen = new AboutScreen(*backHandler, *highScoreHandler, highscores[9], option);
    }
    screen->audioController = audioController;
}

int main(int argc, char *argv[]) {
    #ifdef FORMPU
    stdio_init_all();
    sleep_ms(3000);
    #endif

    srand((unsigned int)time(0));
    readHighScoreData(); 
    Display *display = new Display();
    display->clear(Color(255, 255, 255));

    Battery *battery = new Battery();
    KeyBoard *keyboard = new KeyBoard();
    multicore_launch_core1(&audioControllerCore);
    
    screen = new SplashScreen(*backHandler, *highScoreHandler, 0, 1);
    audioController->setNoteDuration(100);
    std::string song[] = {  "G6", "G6", "P", "G6", "P", "D6", "G6", "P", "A6", "P", "P", "P", "A5", "P", 
                            "P", "P", "P", "D6", "P", "P", "P", "A5", "P", "G5", "P", "P", "C6", "P",
                            "D6", "P", "C6", "C6", "P", "A5", "P", "G6", "A6", "P", "C7", "P", "G6", "A6", 
                            "P", "G6", "P", "D6", "F6", "D6", "P", "P", "D6", "P", "P", "A5", "P", "P", 
                            "G5", "P", "P", "C6", "P", "D6", "P", "C6", "C6", "P6", "A5", "P", "G6", "A6", 
                            "C7", "P", "G6", "A6", "P", "G6", "P", "D6", "F6", "D3"};
    audioController->sing(song, sizeof(song)/sizeof(song[0]));

    timetype lastUpdate = getTime();
    bool close = false;
    while (!close) {
        uint16_t deltaTimeMS = getTimeDiffMS(lastUpdate);
        lastUpdate = getTime();

        screen->update(deltaTimeMS);
        keyboard->checkKeyState(screen);
        screen->draw(display);

        battery->drawLevel(display);
        // printf("FPS: %d\n", int(1000 / deltaTimeMS));
        display->update();
    }

    return EXIT_SUCCESS;
}