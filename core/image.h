#include "common.h"
#include "display.h"

#ifndef _GAME_TIGER_IMAGE_H
#define _GAME_TIGER_IMAGE_H

class Image
{
private:
    uint8_t *palette;
    uint8_t *pixelData;
    uint16_t colorCount;
    std::map<char, std::array<uint8_t, 5> > spriteData;
    uint8_t alpha = 255;
public:
    uint16_t width, height;

    Image(uint16_t width, uint16_t height, uint16_t colorCount, uint8_t *palette, uint8_t *pixelData);
    Image(uint16_t width, uint16_t height, uint16_t colorCount, uint8_t *palette, uint8_t *pixelData, std::map<char, std::array<uint8_t, 5> > spriteData);
    ~Image();

    void setAlpha(uint8_t alpha);
    void draw(Display *display, int16_t screenX, int16_t screenY, uint16_t spriteX = 0, uint16_t spriteY = 0, uint16_t spriteWidth = 0, uint16_t spriteHeight = 0);
    void drawSprite(Display *display, char index, int16_t screenX, int16_t screenY);
    void drawSprites(Display *display, std::string indices, int16_t screenX, int16_t screenY);
    uint16_t getWidth(std::string indices);
};

#endif