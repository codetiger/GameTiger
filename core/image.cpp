#include "image.h"
#include <iostream>
#include <fstream>

Image::Image(Size2 s, uint16_t cc, Color *plt, uint8_t *alphas, uint8_t *pd) {
    this->hasIndexedColors = true;
    this->size = s;
    this->colorCount = cc;
    this->palette = plt;
    this->alphas = alphas;
    this->pixelData = pd;
}

Image::Image(Size2 s, uint16_t cc, Color *plt, uint8_t *alphas, uint8_t *pd, uint16_t *sd) {
    this->hasIndexedColors = true;
    this->size = s;
    this->colorCount = cc;
    this->palette = plt;
    this->alphas = alphas;
    this->pixelData = pd;
    this->spriteData = sd;
}

Image::Image(Size2 s, Color *pd, uint16_t *sd) {
    this->hasIndexedColors = false;
    this->size = s;
    this->palette = pd;
    this->spriteData = sd;
}

void Image::draw(Display *display, Rect2 destRect, Rect2 spriteRect, uint8_t alpha, bool flipH, bool flipV) {
    if(alpha == 0 || (int)destRect.x+(int)destRect.w <= 0 || destRect.x >= DISPLAY_WIDTH || (int)destRect.y+(int)destRect.h <= 0 || destRect.y >= DISPLAY_HEIGHT)
        return;

    uint32_t xRatio = (destRect.w == spriteRect.w) ? 1 : (uint32_t)((spriteRect.w << 16) / destRect.w);
    uint32_t yRatio = (destRect.h == spriteRect.h) ? 1 : (uint32_t)((spriteRect.h << 16) / destRect.h);

    uint16_t fx = 0, fw = destRect.w;
    if(destRect.x < 0) {
        fx = abs(destRect.x) * spriteRect.w / destRect.w;
        fw += destRect.x;
        destRect.x = 0;
    }

    uint16_t fy = 0, fh = destRect.h;
    if(destRect.y < 0) {
        fy = abs(destRect.y) * spriteRect.h / destRect.h;
        fh += destRect.y;
        destRect.y = 0;
    }

    if(!this->hasIndexedColors && !flipH && !flipV && xRatio == yRatio == 1) {
        if(destRect.x+spriteRect.w > DISPLAY_WIDTH)
            fw -= destRect.x+spriteRect.w-DISPLAY_WIDTH;
        if(destRect.y+spriteRect.h > DISPLAY_HEIGHT)
            fh -= destRect.y+spriteRect.h-DISPLAY_HEIGHT;

        for (int y = 0; y < fh; y++) {
            int pixIndex = (fy+spriteRect.y+y) * this->size.w + fx+spriteRect.x;
            display->drawBitmapRow(Vec2(destRect.x, destRect.y + y), fw, &this->palette[pixIndex]);
        }
    } else {
        for (int y = 0; y < fh; y++) {
            for (int x = 0; x < fw; x++) {
                uint16_t cx = (xRatio == 1) ? x : ((x * xRatio) >> 16);
                uint16_t cy = (yRatio == 1) ? y : ((y * yRatio) >> 16);
                uint16_t px = (flipV ? (spriteRect.y+(fh-cy-1)) : (spriteRect.y+fy+cy));
                uint16_t py = (flipH ? (spriteRect.x+(fw-cx-1)) : (spriteRect.x+fx+cx));
                int pixIndex = px * this->size.w + py;
                int colIndex = this->hasIndexedColors ? this->pixelData[pixIndex] : pixIndex;
                uint8_t a = this->hasIndexedColors ? this->alphas[colIndex] : 255;
                a = a > alpha ? alpha : a;
                display->setPixel(Vec2(destRect.x + x, destRect.y + y), this->palette[colIndex], a);
            }
        }
    }
}

void Image::drawSprite(Display *display, uint16_t index, Vec2 destPos, bool flipH, bool flipV) {
    this->drawSprite(display, index, destPos, 255, flipH, flipV);
}

void Image::drawSprite(Display *display, uint16_t index, Vec2 destPos, uint8_t alpha, bool flipH, bool flipV) {
    Size2 spriteSize = Size2(this->getSpriteWidth(index), this->getSpriteHeight(index));
    this->drawSprite(display, index, Rect2(destPos, spriteSize), alpha, flipH, flipV);
}

void Image::drawSprite(Display *display, uint16_t index, Rect2 destRect, uint8_t alpha, bool flipH, bool flipV) {
    Rect2 spriteRect = Rect2(this->getSpriteX(index), this->getSpriteY(index), this->getSpriteWidth(index), this->getSpriteHeight(index));
    this->draw(display, destRect, spriteRect, alpha, flipH, flipV);
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

void Image::drawText(Display *display, std::string text, Vec2 destPos) {
    this->drawText(display, text, destPos, 255);
}

void Image::drawText(Display *display, std::string text, Vec2 destPos, uint8_t alpha) {
    this->drawText(display, text, destPos, alpha, 1);
}

void Image::drawText(Display *display, std::string text, Vec2 destPos, uint8_t alpha, uint8_t scaleRatio) {
    for(char& c : text) {
        uint16_t i = ref.find(c);
        Size2 spriteSize = Size2(this->getSpriteWidth(i), this->getSpriteHeight(i));
        if(c != ' ')
            this->drawSprite(display, i, Rect2(destPos, spriteSize*scaleRatio), alpha);
        destPos.x += (spriteSize.w - 1) * scaleRatio;
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
