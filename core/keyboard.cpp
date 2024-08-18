#include "keyboard.h"

KeyBoard::KeyBoard() {
    printf("[Keyboard] driver loading...\n");
    for (uint8_t i = 0; i < KEY_COUNT; i++)
        this->prevKeyState[i] == false;

    for (uint8_t i = 0; i < KEY_COUNT; i++) {
        gpio_init(pinId[i]);
        gpio_set_dir(pinId[i], GPIO_IN);
        gpio_pull_up(pinId[i]);
    }
    printf("[Keyboard] Done\n");
}

KeyBoard::~KeyBoard() {
}

void KeyBoard::checkKeyState(Screen *screen) {
    for (uint8_t i = 0; i < KEY_COUNT; i++) {
        bool keyState = !gpio_get(pinId[i]);
        if (this->prevKeyState[i] != keyState) {
            if (keyState) {
                screen->keyPressed(i);
                // audio_play_once(snd_drum, sizeof(snd_drum));
            } else
                screen->keyReleased(i);
        } else if(keyState)
            screen->keyDown(i);
        this->prevKeyState[i] = keyState;
    }
}
