#include "core/common.h"
#include "core/display.h"
#include "core/battery.h"
#include "core/keyboard.h"
#include "core/audio.h"
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
AudioController *audioController = new AudioController();
bool shouldSwitchScreen;
uint8_t newScreenId, newOption;

void secondCPUCore() {
#ifdef FORMPU
    printf("[Main] Second CPU Core started\n");
    while (true) {
        // char payload[] = "Hi Lora, GameTiger";
        // lora->SendData(payload, strlen(payload));
        // lora->CheckDeviceStatus();
        // sleep_ms(1000);

        sleep_ms(50);
        lora->ProcessIrq();

        // int32_t num = multicore_fifo_pop_blocking();
        // if(num == AUDIO_FLAG_VALUE)
        //     audioController->play();
    }
#endif
}

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
    screen->audioController = audioController;
    shouldSwitchScreen = false;
}

int main(int argc, char *argv[]) {
    #ifdef FORMPU
    sleep_ms(50);
    vreg_set_voltage(VREG_VOLTAGE_1_20);
    sleep_ms(1);
    set_sys_clock_khz(300 * 1000, true);
    sleep_ms(50);

    stdio_init_all();
    sleep_ms(1000);
    #endif

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
    lora = new Lora();
    lora->SetToReceiveMode();

    #ifdef FORMPU
    multicore_launch_core1(&secondCPUCore);
    #endif
    
    screen = new SplashScreen(*backHandler, *highScoreHandler, 0, 1);
    // audioController->setNoteDuration(100);
    // std::string song[] = {  "G6", "G6", "P", "G6", "P", "D6", "G6", "P", "A6", "P", "P", "P", "A5", "P", 
    //                         "P", "P", "P", "D6", "P", "P", "P", "A5", "P", "G5", "P", "P", "C6", "P",
    //                         "D6", "P", "C6", "C6", "P", "A5", "P", "G6", "A6", "P", "C7", "P", "G6", "A6", 
    //                         "P", "G6", "P", "D6", "F6", "D6", "P", "P", "D6", "P", "P", "A5", "P", "P", 
    //                         "G5", "P", "P", "C6", "P", "D6", "P", "C6", "C6", "P6", "A5", "P", "G6", "A6", 
    //                         "C7", "P", "G6", "A6", "P", "G6", "P", "D6", "F6", "D3"};
    // audioController->sing(song, sizeof(song)/sizeof(song[0]));

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