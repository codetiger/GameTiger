#ifdef FORMPU
    #include "pico/stdlib.h"
    #include "hardware/gpio.h"
    #include "hardware/adc.h"
    #include "hardware/pwm.h"
    #include "hardware/spi.h"
    #include "hardware/dma.h"
    #include "hardware/flash.h"
    #include "hardware/sync.h"

    #define TimeSinceBoot to_ms_since_boot(get_absolute_time())
    #define CLOCKS_PER_SEC 1000
#else
    #include <cstdint>
    #include <SDL.h>

    #define TimeSinceBoot clock()
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
