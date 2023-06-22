#include "keyboard.h"

KeyBoard::KeyBoard() {
    printf("[Keyboard] driver loading...\n");
    for (uint8_t i = 0; i < KEY_COUNT; i++)
        this->prevKeyState[i] == false;
#ifdef FORMPU
    gpio_init(I2CSDA);
    gpio_set_function(I2CSDA, GPIO_FUNC_I2C);
    gpio_pull_up(I2CSDA);

    gpio_init(I2CSCL);
    gpio_set_function(I2CSCL, GPIO_FUNC_I2C);
    gpio_pull_up(I2CSCL);

    int br = i2c_init(i2c1, 200 * 1000);
    printf("[Keyboard] PCF8575 baudrate: %d\n", br);

    uint8_t keystate[2];
    keystate[0] = 0xff;
    keystate[1] = 0xff;
    int ret = i2c_write_blocking(i2c1, ADDR, keystate, 2, false);
    if(ret <= 0)
        printf("[Keyboard] Error: %d\n", ret);
#endif
    printf("[Keyboard] Done\n");
}

KeyBoard::~KeyBoard() {
}

void KeyBoard::checkKeyState(Screen *screen) {
#ifdef FORMPU
    uint8_t keystate[2];
    int ret = i2c_read_blocking(i2c1, ADDR, keystate, 2, false);
    uint state = ~(keystate[0] | (keystate[1] << 8));
    if(ret <= 0) {
        return;
    } else if(screen && state != 0xffff) {
        for (uint8_t i = 0; i < KEY_COUNT; i++) {
            bool keyState = (state >> pinId[i]) & 0x01;
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
