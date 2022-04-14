#include "common.h"

Image alphanumfont = Image(alphanumfont_img_width, alphanumfont_img_height, alphanumfont_color_count, (uint8_t*)alphanumfont_palette, (uint8_t*)alphanumfont_pixel_data, alphanumfont_sprite_data);
Image menuItemLogo[8] = {
    Image(snake_img_width, snake_img_height, snake_color_count, (uint8_t*)snake_palette, (uint8_t*)snake_pixel_data),
    Image(g2048_img_width, g2048_img_height, g2048_color_count, (uint8_t*)g2048_palette, (uint8_t*)g2048_pixel_data),
    Image(tetris_img_width, tetris_img_height, tetris_color_count, (uint8_t*)tetris_palette, (uint8_t*)tetris_pixel_data),
    Image(mine_img_width, mine_img_height, mine_color_count, (uint8_t*)mine_palette, (uint8_t*)mine_pixel_data),
    Image(ttt_img_width, ttt_img_height, ttt_color_count, (uint8_t*)ttt_palette, (uint8_t*)ttt_pixel_data),
    Image(ninjafrog_img_width, ninjafrog_img_height, ninjafrog_color_count, (uint8_t*)ninjafrog_palette, (uint8_t*)ninjafrog_pixel_data),
    Image(settings_img_width, settings_img_height, settings_color_count, (uint8_t*)settings_palette, (uint8_t*)settings_pixel_data),
    Image(about_img_width, about_img_height, about_color_count, (uint8_t*)about_palette, (uint8_t*)about_pixel_data),
};
Image tiger = Image(tiger_img_width, tiger_img_height, tiger_color_count, (uint8_t*)tiger_palette, (uint8_t*)tiger_pixel_data);
Image sweeper = Image(sweeper_img_width, sweeper_img_height, sweeper_color_count, (uint8_t*)sweeper_palette, (uint8_t*)sweeper_pixel_data, sweeper_sprite_data);
Image tttblocks = Image(tttblocks_img_width, tttblocks_img_height, tttblocks_color_count, (uint8_t*)tttblocks_palette, (uint8_t*)tttblocks_pixel_data, tttblocks_sprite_data);

Image pa2terrain = Image(pa2terrain_img_width, pa2terrain_img_height, pa2terrain_color_count, (uint8_t*)pa2terrain_palette, (uint8_t*)pa2terrain_pixel_data, pa2terrain_sprite_data);
Image pa2bg = Image(pa2bg_img_width, pa2bg_img_height, pa2bg_color_count, (uint8_t*)pa2bg_palette, (uint8_t*)pa2bg_pixel_data, pa2bg_sprite_data);
Image pa2fruits = Image(pa2fruits_img_width, pa2fruits_img_height, pa2fruits_color_count, (uint8_t*)pa2fruits_palette, (uint8_t*)pa2fruits_pixel_data, pa2fruits_sprite_data);

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
