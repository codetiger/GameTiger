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

Image::Image(uint16_t w, uint16_t h, uint16_t cc, uint8_t *plt, uint8_t *pd, std::map<char, std::array<uint8_t, 5> > sd){
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

void Image::draw(Display *display, int16_t screenX, int16_t screenY, uint16_t spriteX, uint16_t spriteY, uint16_t spriteWidth, uint16_t spriteHeight) {
    if(this->alpha == 0)
        return;

    spriteWidth = (spriteWidth == 0) ? this->width : spriteWidth;
    spriteHeight = (spriteHeight == 0) ? this->height : spriteHeight;

    if(screenX+spriteWidth <= 0 || screenX > display->width || screenY+spriteHeight <= 0 || screenY > display->height)
        return;

    for (int y = 0; y < spriteHeight; y++) {
        for (int x = 0; x < spriteWidth; x++) {
            if (x+screenX < 0 || x+screenX >= display->width || y+screenY < 0 || y+screenY >= display->height) 
                continue;

            int pixIndex = (y+spriteY) * this->width + (x+spriteX);
            int colIndex = this->pixelData[pixIndex] * 4;
            Color c = Color(this->palette[colIndex], this->palette[colIndex+1], this->palette[colIndex+2]);
            uint8_t alpha = std::min(this->palette[colIndex+3], this->alpha);
            if(alpha > 0)
                display->setPixel(x+screenX, y+screenY, c, alpha);
        }
    }
}

void Image::drawSprite(Display *display, char index, int16_t screenX, int16_t screenY) {
    if(this->alpha == 0)
        return;

    this->draw(display, screenX, screenY, this->spriteData[index][0], this->spriteData[index][1], this->spriteData[index][2], this->spriteData[index][3]);
}

void Image::drawSprites(Display *display, std::string indices, int16_t screenX, int16_t screenY) {
    if(this->alpha == 0)
        return;

    uint16_t posX = screenX;
    for(char& index : indices) {
        this->draw(display, posX, screenY, this->spriteData[index][0], this->spriteData[index][1], this->spriteData[index][2], this->spriteData[index][3]);
        posX += this->spriteData[index][2] - 1;
    }
}

uint16_t Image::getWidth(std::string indices) {
    uint16_t width = 0;
    for(char& index : indices)
        width += this->spriteData[index][2] - 1;
    return width;
}

Image::~Image() {
    delete[] this->palette;
    delete[] this->pixelData;
}
