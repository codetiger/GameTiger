#include "keyboard.h"

KeyBoard::KeyBoard() {
    for (int i = 0; i < 6; i++) {
        this->prevKeyState[i] = false;
        gpio_init(this->gpioPins[i]);
        gpio_set_dir(this->gpioPins[i], GPIO_IN);
        gpio_pull_down(this->gpioPins[i]);
    }
}

KeyBoard::~KeyBoard() {
}

void KeyBoard::checkKeyState(Screen *screen) {
    for (uint8_t i = 0; i < 6; i++) {
        bool keyState = gpio_get(this->gpioPins[i]);
        if (this->prevKeyState[i] != keyState) {
            if (keyState) 
                screen->keyPressed(i);
            else
                screen->keyReleased(i);
        } else if(keyState)
            screen->keyDown(i);
        this->prevKeyState[i] = keyState;
    }
}
