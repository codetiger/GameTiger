#include "common.h"
#include "color.h"

#ifndef _GAME_TIGER_DISPLAY_H
#define _GAME_TIGER_DISPLAY_H

class Display {
private:
    const uint8_t CS_PIN = 9;
    const uint8_t BL_PIN = 13;
    const uint8_t DC_PIN = 8;
    const uint8_t RST_PIN = 12;
    const uint8_t MOSI_PIN = 11;
    const uint8_t SCK_PIN = 10;

#ifdef FORMPU
    void write_cmd(const uint8_t cmd);
    void write_data(const uint8_t data);
    void write_data(const uint8_t data[]);
    void sendData(const uint8_t cmd, const uint8_t data[]);
    void sendData(const uint8_t cmd, const uint8_t data);
    void setWindow(const uint16_t x, const uint16_t y, const uint16_t width, const uint16_t height);
    void setCursor(const uint16_t x, const uint16_t y);

    void initHardware();
    void initSequence();
    void reset();
#else
    SDL_Renderer* renderer;
    SDL_Window* window;
#endif

    Color buffer[320*240];

#ifdef FORMPU
    int dmaSPIChannel = 0;
    dma_channel_config dmaSPIConfig;

    int dmaMemChannel = 0;
    dma_channel_config dmaMemConfig;
#endif
public:
    const uint16_t width = 320, height = 240;

    Display();
    ~Display();

    void update();
    void clear(Color c);
    void setBrightness(uint8_t brightness);

    void setPixel(int x, int y, Color c, uint8_t alpha);
    void fillRect(int x, int y, int width, int height, Color c, uint8_t alpha = 255);
    void hLine(int x, int y, int width, Color c, uint8_t alpha = 255);
    void vLine(int x, int y, int height, Color c, uint8_t alpha = 255);
    void rect(int x, int y, int width, int height, Color c, uint8_t alpha = 255);
};

#endif