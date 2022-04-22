#include "image.h"
#include <iostream>
#include <fstream>

Image::Image(uint16_t w, uint16_t h, uint16_t cc, Color *plt, uint8_t *alphas, uint8_t *pd) {
    this->hasIndexedColors = true;
    this->width = w;
    this->height = h;
    this->colorCount = cc;
    this->palette = plt;
    this->alphas = alphas;
    this->pixelData = pd;
}

Image::Image(uint16_t w, uint16_t h, uint16_t cc, Color *plt, uint8_t *alphas, uint8_t *pd, uint16_t *sd) {
    this->hasIndexedColors = true;
    this->width = w;
    this->height = h;
    this->colorCount = cc;
    this->palette = plt;
    this->alphas = alphas;
    this->pixelData = pd;
    this->spriteData = sd;
}

Image::Image(uint16_t w, uint16_t h, Color *pd, uint16_t *sd) {
    this->hasIndexedColors = false;
    this->width = w;
    this->height = h;
    this->palette = pd;
    this->spriteData = sd;
}

void Image::draw(Display *display, int16_t screenX, int16_t screenY, uint16_t spriteX, uint16_t spriteY, uint16_t spriteWidth, uint16_t spriteHeight, int8_t scale, uint8_t alpha, bool flipH, bool flipV) {
    spriteWidth = (spriteWidth == 0) ? this->width : spriteWidth;
    spriteHeight = (spriteHeight == 0) ? this->height : spriteHeight;

    if(alpha == 0 || screenX+spriteWidth <= 0 || screenX >= DISPLAY_WIDTH || screenY+spriteHeight <= 0 || screenY >= DISPLAY_HEIGHT)
        return;
    if(screenX < 0) {
        spriteX -= screenX;
        spriteWidth += screenX;
        screenX = 0;
    }
    if(screenY < 0) {
        spriteY -= screenY;
        spriteHeight += screenY;
        screenY = 0;
    }
    if(screenX+spriteWidth > DISPLAY_WIDTH)
        spriteWidth -= (screenX+spriteWidth) - DISPLAY_WIDTH;
    if(screenY+spriteHeight > DISPLAY_HEIGHT)
        spriteHeight -= (screenY+spriteHeight) - DISPLAY_HEIGHT;

    if(!this->hasIndexedColors && !flipH && !flipV && scale == 1) {
        for (int y = 0; y < spriteHeight; y++) {
            int pixIndex = (spriteY+y) * this->width + spriteX;
            display->drawBitmapRow(screenX, screenY + y, spriteWidth, &this->palette[pixIndex]);
        }
    } else {
        for (int y = 0; y < spriteHeight; y++) {
            for (int x = 0; x < spriteWidth; x++) {
                int pixIndex = (spriteY+(flipV ? spriteHeight-y-1 : y)) * this->width + (spriteX + (flipH ? spriteWidth-x-1 : x));
                int colIndex = this->hasIndexedColors ? this->pixelData[pixIndex] : pixIndex;
                uint8_t a = this->hasIndexedColors ? this->alphas[colIndex] : 255;
                a = a > alpha ? alpha : a;
                for(int sx = 0; sx < scale; sx++)
                    for(int sy = 0; sy < scale; sy++)
                        display->setPixel((x*scale)+screenX+sx, (y*scale)+screenY+sy, this->palette[colIndex], a);
            }
        }
    }
}

void Image::drawSprite(Display *display, uint16_t index, int16_t screenX, int16_t screenY, int8_t scale, uint8_t alpha, bool flipH, bool flipV) {
    if(alpha == 0)
        return;

    this->draw(display, screenX, screenY, this->spriteData[index*4+0], this->spriteData[index*4+1], this->spriteData[index*4+2], this->spriteData[index*4+3], scale, alpha, flipH, flipV);
}

void Image::drawSprites(Display *display, std::string indices, int16_t screenX, int16_t screenY, int8_t scale, uint8_t alpha, bool flipH, bool flipV) {
    if(alpha == 0)
        return;

    uint16_t posX = screenX;
    for(char& index : indices) {
        uint16_t i = ref.find(index);
        if(index != ' ')
            this->draw(display, posX, screenY, this->spriteData[i*4+0], this->spriteData[i*4+1], this->spriteData[i*4+2], this->spriteData[i*4+3], scale, alpha, flipH, flipV);
        posX += (this->spriteData[i*4+2] - 1) * scale;
    }
}

uint16_t Image::getWidth(std::string indices, int8_t scale) {
    uint16_t width = 0;
    for(char& index : indices) {
        uint16_t i = ref.find(index);
        width += this->getSpriteWidth(i) * scale - 1;
    }
    return width;
}

uint16_t Image::getSpriteWidth(uint16_t index) {
    return this->spriteData[index*4+2];
}

uint16_t Image::getSpriteHeight(uint16_t index) {
    return this->spriteData[index*4+3];
}
