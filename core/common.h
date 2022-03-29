#ifdef FORMPU
    #include "pico/stdlib.h"
    #include "hardware/gpio.h"
    #include "hardware/adc.h"
    #include "hardware/pwm.h"
    #include "hardware/spi.h"
    #include "hardware/dma.h"
    #include "hardware/flash.h"
    #include "hardware/sync.h"

    #define timetype uint32_t
#else
    #include <cstdint>
    #include <SDL.h>
    #include <chrono>

    typedef std::chrono::high_resolution_clock Clock;
    typedef std::chrono::milliseconds milliseconds;
    #define timetype Clock::time_point
    #define FLASH_PAGE_SIZE (1u << 8)
#endif

#include <map>
#include <string>
#include <cstring>
#include <array>
#include <math.h>
#include <algorithm>
#include <string> 
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#ifndef _GAME_TIGER_COMMON_H
#define _GAME_TIGER_COMMON_H

enum GameState {WAITING, PLAYING, LOST};
timetype getTime();
uint16_t getTimeDiffMS(timetype start);

#endif