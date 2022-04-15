#include "common.h"
#include "display.h"

#ifndef _GAME_TIGER_IMAGE_H
#define _GAME_TIGER_IMAGE_H

struct Image
{
private:
    uint8_t *palette;
    uint8_t *pixelData;
    uint16_t colorCount;
    std::map<int16_t, std::array<uint16_t, 5> > spriteData;
public:
    uint16_t width, height;

    Image(uint16_t width, uint16_t height, uint16_t colorCount, uint8_t *palette, uint8_t *pixelData);
    Image(uint16_t width, uint16_t height, uint16_t colorCount, uint8_t *palette, uint8_t *pixelData, std::map<int16_t, std::array<uint16_t, 5> > spriteData);

    void draw(Display *display, int16_t screenX, int16_t screenY, uint16_t spriteX = 0, uint16_t spriteY = 0, uint16_t spriteWidth = 0, uint16_t spriteHeight = 0, int8_t scale = 1, uint8_t alpha = 255, bool flipH = false, bool flipV = false);
    void drawSprite(Display *display, char index, int16_t screenX, int16_t screenY, int8_t scale = 1, uint8_t alpha = 255, bool flipH = false, bool flipV = false);
    void drawSprites(Display *display, std::string indices, int16_t screenX, int16_t screenY, int8_t scale = 1, uint8_t alpha = 255, bool flipH = false, bool flipV = false);
    uint16_t getWidth(std::string indices, int8_t scale = 1);
    uint16_t getSpriteWidth(char index);
    uint16_t getSpriteHeight(char index);
};
typedef struct Image Image;

#endif