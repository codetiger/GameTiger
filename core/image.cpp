#include "image.h"
#include <iostream>
#include <fstream>

Image::Image(uint16_t w, uint16_t h, uint16_t cc, uint8_t *plt, uint8_t *pd) {
    this->width = w;
    this->height = h;
    this->colorCount = cc;
    this->palette = plt;
    this->pixelData = pd;
}

Image::Image(uint16_t w, uint16_t h, uint16_t cc, uint8_t *plt, uint8_t *pd, std::map<int16_t, std::array<uint16_t, 5> > sd){
    this->width = w;
    this->height = h;
    this->colorCount = cc;
    this->palette = plt;
    this->pixelData = pd;
    this->spriteData = sd;
}

void Image::setAlpha(uint8_t a) {
    this->alpha = a;
}

void Image::draw(Display *display, int16_t screenX, int16_t screenY, uint16_t spriteX, uint16_t spriteY, uint16_t spriteWidth, uint16_t spriteHeight, int8_t scale) {
    spriteWidth = (spriteWidth == 0) ? this->width : spriteWidth;
    spriteHeight = (spriteHeight == 0) ? this->height : spriteHeight;

    if(this->alpha == 0 || screenX+spriteWidth <= 0 || screenX >= DISPLAY_WIDTH || screenY+spriteHeight <= 0 || screenY >= DISPLAY_HEIGHT)
        return;

    for (int y = 0; y < spriteHeight; y++) {
        for (int x = 0; x < spriteWidth; x++) {
            if (x+screenX < 0 || x+screenX >= DISPLAY_WIDTH || y+screenY < 0 || y+screenY >= DISPLAY_HEIGHT) 
                continue;

            int pixIndex = (y+spriteY) * this->width + (x+spriteX);
            int colIndex = this->pixelData[pixIndex] * 4;
            uint8_t alpha = this->palette[colIndex+3];
            if(alpha > 0) {
                Color c = Color(this->palette[colIndex], this->palette[colIndex+1], this->palette[colIndex+2]);
                alpha = alpha > this->alpha ? this->alpha : alpha;
                for(int sx = 0; sx < scale; sx++)
                    for(int sy = 0; sy < scale; sy++)
                        display->setPixel((x*scale)+screenX+sx, (y*scale)+screenY+sy, c, alpha);
            }
        }
    }
}

void Image::drawSprite(Display *display, char index, int16_t screenX, int16_t screenY, int8_t scale) {
    if(this->alpha == 0)
        return;

    this->draw(display, screenX, screenY, this->spriteData[index][0], this->spriteData[index][1], this->spriteData[index][2], this->spriteData[index][3], scale);
}

void Image::drawSprites(Display *display, std::string indices, int16_t screenX, int16_t screenY, int8_t scale) {
    if(this->alpha == 0)
        return;

    uint16_t posX = screenX;
    for(char& index : indices) {
        if(index != ' ')
            this->draw(display, posX, screenY, this->spriteData[index][0], this->spriteData[index][1], this->spriteData[index][2], this->spriteData[index][3], scale);
        posX += (this->spriteData[index][2] - 1) * scale;
    }
}

uint16_t Image::getWidth(std::string indices, int8_t scale) {
    uint16_t width = 0;
    for(char& index : indices)
        width += this->spriteData[index][2] * scale - 1;
    return width;
}

uint16_t Image::getSpriteWidth(char index) {
    return this->spriteData[index][2];
}

uint16_t Image::getSpriteHeight(char index) {
    return this->spriteData[index][3];
}
