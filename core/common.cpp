#include "common.h"

Image alphanumfont = Image(alphanumfont_img_width, alphanumfont_img_height, alphanumfont_color_count, (uint8_t*)alphanumfont_palette, (uint8_t*)alphanumfont_pixel_data, alphanumfont_sprite_data);
Image allGameSprite = Image(allGameSprite_img_width, allGameSprite_img_height, allGameSprite_color_count, (uint8_t*)allGameSprite_palette, (uint8_t*)allGameSprite_pixel_data, allGameSprite_sprite_data);
Image menuSprite = Image(menuSprite_img_width, menuSprite_img_height, menuSprite_color_count, (uint8_t*)menuSprite_palette, (uint8_t*)menuSprite_pixel_data, menuSprite_sprite_data);

timetype getTime() {
    timetype now;
#ifdef FORMPU
    now = to_ms_since_boot(get_absolute_time());
#else
    now = Clock::now();
#endif
    // printf("Time: %d\n", now);
    return now;
}

uint16_t getTimeDiffMS(timetype start) {
    timetype end = getTime();
    uint16_t timeDiffMS = 0;
#ifdef FORMPU
    timeDiffMS = (end - start);
#else
    milliseconds ms = std::chrono::duration_cast<milliseconds>(end - start);
    timeDiffMS = ms.count();
#endif
    // printf("Time diff: %d\n", timeDiffMS);
    return timeDiffMS;
}
