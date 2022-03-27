#include "keyboard.h"

KeyBoard::KeyBoard() {
#ifdef FORMPU
    for (int i = 0; i < 6; i++) {
        this->prevKeyState[i] = false;
        gpio_init(this->gpioPins[i]);
        gpio_set_dir(this->gpioPins[i], GPIO_IN);
        gpio_pull_down(this->gpioPins[i]);
    }
#endif
}

KeyBoard::~KeyBoard() {
}

void KeyBoard::checkKeyState(Screen *screen) {
#ifdef FORMPU
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
#else
    SDL_Event event; 
    while (SDL_PollEvent(&event)) {
        if(event.type == SDL_QUIT || (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_ESCAPE)) {
            exit(EXIT_SUCCESS);
        } else if(event.type == SDL_KEYDOWN) {
            if(event.key.keysym.sym == SDLK_UP) {
                screen->keyPressed(KEY_UP);
            } else if(event.key.keysym.sym == SDLK_DOWN) {
                screen->keyPressed(KEY_DOWN);
            } else if(event.key.keysym.sym == SDLK_RIGHT) {
                screen->keyPressed(KEY_RIGHT);
            } else if(event.key.keysym.sym == SDLK_LEFT) {
                screen->keyPressed(KEY_LEFT);
            } else if(event.key.keysym.sym == SDLK_a) {
                screen->keyPressed(KEY_A);
            } else if(event.key.keysym.sym == SDLK_b) {
                screen->keyPressed(KEY_B);
            }
        }
    }

#endif
}
