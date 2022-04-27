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

void Image::draw(Display *display, int16_t destX, int16_t destY, uint16_t destWidth, uint16_t destHeight, 
                                    uint16_t spriteX, uint16_t spriteY, uint16_t spriteWidth, uint16_t spriteHeight, 
                                    uint8_t alpha, bool flipH, bool flipV) {
    if(alpha == 0 || destX+destWidth <= 0 || destX >= DISPLAY_WIDTH || destY+destHeight <= 0 || destY >= DISPLAY_HEIGHT)
        return;

    uint32_t xRatio = (destWidth == spriteWidth) ? 1 : (uint32_t)((spriteWidth << 16) / destWidth);
    uint32_t yRatio = (destHeight == spriteHeight) ? 1 : (uint32_t)((spriteHeight << 16) / destHeight);

    uint16_t fx = 0, fw = destWidth;
    if(destX < 0) {
        fx = abs(destX) * spriteWidth / destWidth;
        fw += destX;
        destX = 0;
    }

    uint16_t fy = 0, fh = destHeight;
    if(destY < 0) {
        fy = abs(destY) * spriteHeight / destHeight;
        fh += destY;
        destY = 0;
    }

    if(!this->hasIndexedColors && !flipH && !flipV && xRatio == yRatio == 1) {
        if(destX+spriteWidth > DISPLAY_WIDTH)
            fw -= destX+spriteWidth-DISPLAY_WIDTH;
        if(destY+spriteHeight > DISPLAY_HEIGHT)
            fh -= destY+spriteHeight-DISPLAY_HEIGHT;

        for (int y = 0; y < fh; y++) {
            int pixIndex = (fy+spriteY+y) * this->width + fx+spriteX;
            display->drawBitmapRow(destX, destY + y, fw, &this->palette[pixIndex]);
        }
    } else {
        for (int y = 0; y < fh; y++) {
            for (int x = 0; x < fw; x++) {
                uint16_t cx = (xRatio == 1) ? x : ((x * xRatio) >> 16);
                uint16_t cy = (yRatio == 1) ? y : ((y * yRatio) >> 16);
                uint16_t px = (flipV ? (spriteY+(fh-cy-1)) : (spriteY+fy+cy));
                uint16_t py = (flipH ? (spriteX+(fw-cx-1)) : (spriteX+fx+cx));
                int pixIndex = px * this->width + py;
                int colIndex = this->hasIndexedColors ? this->pixelData[pixIndex] : pixIndex;
                uint8_t a = this->hasIndexedColors ? this->alphas[colIndex] : 255;
                a = a > alpha ? alpha : a;
                display->setPixel(destX + x, destY + y, this->palette[colIndex], a);
            }
        }
    }
}

void Image::drawSprite(Display *display, uint16_t index, int16_t destX, int16_t destY, bool flipH, bool flipV) {
    this->drawSprite(display, index, destX, destY, 255, flipH, flipV);
}

void Image::drawSprite(Display *display, uint16_t index, int16_t destX, int16_t destY, uint8_t alpha, bool flipH, bool flipV) {
    uint16_t spriteWidth = this->getSpriteWidth(index);
    uint16_t spriteHeight = this->getSpriteHeight(index);
    this->drawSprite(display, index, destX, destY, spriteWidth, spriteHeight, alpha, flipH, flipV);
}

void Image::drawSprite(Display *display, uint16_t index, int16_t destX, int16_t destY, uint16_t destWidth, uint16_t destHeight, uint8_t alpha, bool flipH, bool flipV) {
    uint16_t spriteX = this->getSpriteX(index);
    uint16_t spriteY = this->getSpriteY(index);
    uint16_t spriteWidth = this->getSpriteWidth(index);
    uint16_t spriteHeight = this->getSpriteHeight(index);
    this->draw(display, destX, destY, destWidth, destHeight, spriteX, spriteY, spriteWidth, spriteHeight, alpha, flipH, flipV);
}

uint16_t Image::getSpriteX(uint16_t index) {
    return this->spriteData[index*4+0];
}

uint16_t Image::getSpriteY(uint16_t index) {
    return this->spriteData[index*4+1];
}

uint16_t Image::getSpriteWidth(uint16_t index) {
    return this->spriteData[index*4+2];
}

uint16_t Image::getSpriteHeight(uint16_t index) {
    return this->spriteData[index*4+3];
}

void Image::drawText(Display *display, std::string text, int16_t destX, int16_t destY) {
    this->drawText(display, text, destX, destY, 255);
}

void Image::drawText(Display *display, std::string text, int16_t destX, int16_t destY, uint8_t alpha) {
    this->drawText(display, text, destX, destY, alpha, 1);
}

void Image::drawText(Display *display, std::string text, int16_t destX, int16_t destY, uint8_t alpha, uint8_t scaleRatio) {
    uint16_t posX = destX;
    for(char& c : text) {
        uint16_t i = ref.find(c);
        uint16_t spriteWidth = this->getSpriteWidth(i);
        uint16_t spriteHeight = this->getSpriteHeight(i);
        if(c != ' ')
            this->drawSprite(display, i, posX, destY, spriteWidth*scaleRatio, spriteHeight*scaleRatio, alpha);
        posX += (spriteWidth - 1) * scaleRatio;
    }
}

uint16_t Image::getTextWidth(std::string text) {
    uint16_t width = 0;
    for(char& c : text) {
        uint16_t i = ref.find(c);
        width += this->getSpriteWidth(i) - 1;
    }
    return width;
}

uint16_t Image::getTextWidth(std::string text, uint8_t scaleRatio) {
    uint16_t width = 0;
    for(char& c : text) {
        uint16_t i = ref.find(c);
        width += (this->getSpriteWidth(i) - 1)*scaleRatio;
    }
    return width;
}