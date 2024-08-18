#include "common.h"

Color BLACKCOLOR = Color((uint16_t)0x0);
Color WHITECOLOR = Color((uint16_t)0xffff);
Color REDCOLOR = Color(255, 0, 0);
Color GREENCOLOR = Color(0, 255, 0);

Image alphanumfont = Image(Size2(alphanumfont_img_width, alphanumfont_img_height), alphanumfont_color_count, (Color*)alphanumfontPaletteColors, (uint8_t*)alphanumfontPaletteAlphas, (uint8_t*)alphanumfont_pixel_data, (uint16_t*)alphanumfontSpriteData);
Image allGameAlphaSprite = Image(Size2(allGameAlphaSpriteWidth, allGameAlphaSpriteHeight), allGameAlphaSpriteColorCount, (Color*)allGameAlphaSpritePaletteColors, (uint8_t*)allGameAlphaSpritePaletteAlphas, (uint8_t*)allGameAlphaSpritePixelData, (uint16_t*)allGameAlphaSpriteSpriteData);
Image menuSprite = Image(Size2(menuSpriteWidth, menuSpriteHeight), menuSpriteColorCount, (Color*)menuSpritePaletteColors, (uint8_t*)menuSpritePaletteAlphas, (uint8_t*)menuSpritePixelData, (uint16_t*)menuSpriteSpriteData);
Image allGameSprite = Image(Size2(allGameSpriteWidth, allGameSpriteHeight), (Color*)allGameSpritePixelData, (uint16_t*)allGameSpriteSpriteData);
Image logoSprite = Image(Size2(logoSpriteWidth, logoSpriteHeight), logoSpriteColorCount, (Color*)logoSpritePaletteColors, (uint8_t*)logoSpritePaletteAlphas, (uint8_t*)logoSpritePixelData, (uint16_t*)logoSpriteSpriteData);

timetype getTime() {
    timetype now;
    now = to_ms_since_boot(get_absolute_time());
    // printf("Time: %d\n", now);
    return now;
}

uint16_t getTimeDiffMS(timetype start) {
    timetype end = getTime();
    uint16_t timeDiffMS = 0;
    timeDiffMS = (end - start);
    // printf("Time diff: %d\n", timeDiffMS);
    return timeDiffMS;
}
