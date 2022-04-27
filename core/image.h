#include "common.h"
#include "display.h"

#ifndef _GAME_TIGER_IMAGE_H
#define _GAME_TIGER_IMAGE_H

const std::string ref = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz ";

struct Image
{
private:
    bool hasIndexedColors;
    Color *palette;
    uint8_t *alphas;
    uint8_t *pixelData;
    uint16_t colorCount;
    uint16_t *spriteData;
    uint16_t width, height;

    uint16_t getSpriteX(uint16_t index);
    uint16_t getSpriteY(uint16_t index);
    void draw(Display *display, int16_t destX, int16_t destY, uint16_t destWidth, uint16_t destHeight, uint16_t spriteX, uint16_t spriteY, uint16_t spriteWidth, uint16_t spriteHeight, uint8_t alpha, bool flipH, bool flipV);
public:

    Image(uint16_t width, uint16_t height, uint16_t colorCount, Color *palette, uint8_t *alphas, uint8_t *pixelData);
    Image(uint16_t width, uint16_t height, uint16_t colorCount, Color *palette, uint8_t *alphas, uint8_t *pixelData, uint16_t *spriteData);
    Image(uint16_t width, uint16_t height, Color *pixelData, uint16_t *spriteData);

    void drawText(Display *display, std::string text, int16_t destX, int16_t destY);
    void drawText(Display *display, std::string text, int16_t destX, int16_t destY, uint8_t alpha);
    void drawText(Display *display, std::string text, int16_t destX, int16_t destY, uint8_t alpha, uint8_t scaleRatio);
    uint16_t getTextWidth(std::string text);
    uint16_t getTextWidth(std::string text, uint8_t scaleRatio);

    void drawSprite(Display *display, uint16_t index, int16_t destX, int16_t destY, bool flipH = false, bool flipV = false);
    void drawSprite(Display *display, uint16_t index, int16_t destX, int16_t destY, uint8_t alpha, bool flipH = false, bool flipV = false);
    void drawSprite(Display *display, uint16_t index, int16_t destX, int16_t destY, uint16_t destWidth, uint16_t destHeight, uint8_t alpha, bool flipH = false, bool flipV = false);
    uint16_t getSpriteWidth(uint16_t index);
    uint16_t getSpriteHeight(uint16_t index);
};
typedef struct Image Image;

#endif