#include "keyboard.h"

KeyBoard::KeyBoard() {
    printf("[Keyboard] driver loading...\n");
    for (uint8_t i = 0; i < KEY_COUNT; i++)
        this->prevKeyState[i] == false;

#ifdef FORMPU
    for (uint8_t i = 0; i < KEY_COUNT; i++) {
        gpio_init(pinId[i]);
        gpio_set_dir(pinId[i], GPIO_IN);
        gpio_pull_up(pinId[i]);
    }
#endif
    printf("[Keyboard] Done\n");
}

KeyBoard::~KeyBoard() {
}

void KeyBoard::checkKeyState(Screen *screen) {
#ifdef FORMPU
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
#else
    SDL_Event event; 
    while (SDL_PollEvent(&event)) {
        if(event.type == SDL_QUIT || (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_ESCAPE)) {
            exit(EXIT_SUCCESS);
        } else if(event.type == SDL_KEYUP) {
            if(event.key.keysym.sym == SDLK_UP) {
                screen->keyReleased(KEY_UP);
            } else if(event.key.keysym.sym == SDLK_DOWN) {
                screen->keyReleased(KEY_DOWN);
            } else if(event.key.keysym.sym == SDLK_RIGHT) {
                screen->keyReleased(KEY_RIGHT);
            } else if(event.key.keysym.sym == SDLK_LEFT) {
                screen->keyReleased(KEY_LEFT);
            } else if(event.key.keysym.sym == SDLK_a) {
                screen->keyReleased(KEY_A);
            } else if(event.key.keysym.sym == SDLK_b) {
                screen->keyReleased(KEY_B);
            }
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
