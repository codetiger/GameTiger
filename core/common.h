    #include "pico/stdlib.h"
    #include "pico/multicore.h"
    #include "hardware/gpio.h"
    #include "hardware/adc.h"
    #include "hardware/pwm.h"
    #include "hardware/spi.h"
    #include "hardware/dma.h"
    #include "hardware/flash.h"
    #include "hardware/sync.h"
    #include "hardware/clocks.h"
    #include "hardware/i2c.h"
    #include "hardware/vreg.h"
    #include "hardware/pll.h"
    
    #define timetype uint32_t

#include <map>
#include <string>
#include <cstring>
#include <array>
#include <math.h>
#include <algorithm>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <queue>

#ifndef _GAME_TIGER_COMMON_H
#define _GAME_TIGER_COMMON_H

#define DISPLAY_WIDTH 320
#define DISPLAY_HEIGHT 240

#define ENABLE_BATTERY_MONITOR

#include "intmath.h"

#include "color.h"
#include "../content/allGameSprite.h"
#include "../content/allGameAlphaSprite.h"
#include "../content/menuSprite.h"
#include "../content/logoSprite.h"
#include "../content/alphanumfont.h"

#include "image.h"
#include "tilemap.h"
#include "level.h"

#define AUDIO_PIN 5

enum ImageType {INDEXEDCOLORS, PIXELCOLOR};
enum GameState {WAITING, PLAYING, PAUSED, LOST};

timetype getTime();
uint16_t getTimeDiffMS(timetype start);

extern Color BLACKCOLOR;
extern Color WHITECOLOR;
extern Color REDCOLOR;
extern Color GREENCOLOR;

extern Image alphanumfont;
extern Image allGameSprite;
extern Image allGameAlphaSprite;
extern Image menuSprite;
extern Image logoSprite;
#endif