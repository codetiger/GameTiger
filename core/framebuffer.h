#include "common.h"
#include "color.h"

#ifndef _GAME_TIGER_FRAMEBUFFER_H
#define _GAME_TIGER_FRAMEBUFFER_H

#define MAX_DMA_CPY_CHN 4
#define MAX_DMA_FILL_CHN 4

enum ColorFormat {
    RGB444 = 12, 
    RGB565 = 16, 
    RGB666 = 18
};

class FrameBuffer {
private:
    uint dmaCopyChannel[MAX_DMA_CPY_CHN];
    dma_channel_config dmaCopyConfig[MAX_DMA_CPY_CHN];
    uint dmaFillChannel[MAX_DMA_FILL_CHN];
    dma_channel_config dmaFillConfig[MAX_DMA_FILL_CHN];

    uint getFreeFillChannelIndex();
    uint getFreeCopyChannelIndex();
public:
    Color *buffer;

    FrameBuffer(uint16_t width, uint16_t height);
    ~FrameBuffer();
    
    void waitOnBusy();
    void clear(Color c);
    void setPixel(Vec2 pos, Color &c, uint8_t alpha);
    void drawBitmapRow(Vec2 pos, int width, Color *c);
    void fillRect(Rect2 rect, Color &c, uint8_t alpha = 255);
    void hLine(Vec2 pos, int width, Color &c, uint8_t alpha = 255);
    void vLine(Vec2 pos, int height, Color &c, uint8_t alpha = 255);
    void rect(Rect2 rect, Color &c, uint8_t alpha = 255);
    void line(Vec2 p0, Vec2 p1, Color &c, uint8_t alpha = 255);
    void triangle(Vec2 p0, Vec2 p1, Vec2 p2, Color &c, uint8_t alpha = 255);
    void fillTriangle(Vec2 p0, Vec2 p1, Vec2 p2, Color &c, uint8_t alpha = 255);
};

#endif