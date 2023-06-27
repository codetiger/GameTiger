#include "display.h"
#include <stdio.h>
#include <algorithm>
#include <memory>

Display::Display() {
    printf("[Display] driver loading...\n");
    this->frameBuffer = new FrameBuffer(DISPLAY_WIDTH, DISPLAY_HEIGHT);
    this->initHardware();
    printf("[Display] Done\n");
}

void Display::initHardware() {
    gpio_init(CS_PIN);
    gpio_set_dir(CS_PIN, GPIO_OUT);

    gpio_init(RST_PIN);
    gpio_set_dir(RST_PIN, GPIO_OUT);

    gpio_init(DC_PIN);
    gpio_set_dir(DC_PIN, GPIO_OUT);

    gpio_init(BL_PIN);
    gpio_set_dir(BL_PIN, GPIO_OUT);
    gpio_put(BL_PIN, 1);

    spi_init(spi1, 120 * 1000 * 1000); //62.5
    uint br = spi_get_baudrate(spi1);
    printf("[Display] baudrate: %d\n", br);

    gpio_set_function(SCK_PIN, GPIO_FUNC_SPI);
    gpio_set_function(MOSI_PIN, GPIO_FUNC_SPI);
}

void Display::initDMAChannel() { 
    this->dmaSPIChannel = dma_claim_unused_channel(true);
    this->dmaSPIConfig = dma_channel_get_default_config(this->dmaSPIChannel);
    channel_config_set_transfer_data_size(&this->dmaSPIConfig, DMA_SIZE_16);
    channel_config_set_read_increment(&this->dmaSPIConfig, true);
    channel_config_set_write_increment(&this->dmaSPIConfig, false);
    channel_config_set_ring(&this->dmaSPIConfig, false, 0);
    channel_config_set_dreq(&this->dmaSPIConfig, DREQ_SPI1_TX);
}

void Display::initSequence() {
    this->reset();
    gpio_put(CS_PIN, 0);

    this->sendData(ST7789_MADCTL, (uint8_t)0xB8);  //0xB8 or 0x78 rotation
    this->sendData(ST7789_COLMOD, 0x55);
    this->sendData(ST7789_INVON);

    uint8_t buf1[] = {0x00, 0x00, 0x01, 0x3F};
    this->sendData(ST7789_CASET, buf1);

    uint8_t buf2[] = {0x00, 0x00, 0x00, 0xEF};
    this->sendData(ST7789_RASET, buf2);

    uint8_t buf3[] = {0x00, 0xE0};
    this->sendData(ST7789_RAMCTRL, buf3);

    uint8_t buf4[] = {0x0C, 0x0C, 0x00, 0x33, 0x33};
    this->sendData(ST7789_PORCTRL, buf4);

    this->sendData(ST7789_GCTRL, 0x35);
    this->sendData(ST7789_VCOMS, 0x28);
    this->sendData(ST7789_LCMCTRL, 0x2C);
    uint8_t buf5[] = {0x01, 0xFF};
    this->sendData(ST7789_VDVVRHEN, buf5);
    this->sendData(ST7789_VRHS, 0x12);
    this->sendData(ST7789_VDVSET, 0x20);
    this->sendData(ST7789_FRCTR2, 0x0F);

    uint8_t buf6[] = {0xA4, 0xA1};
    this->sendData(ST7789_PWCTRL1, buf6);

    uint8_t buf7[] = {0xD0, 0x08, 0x11, 0x08, 0x0C, 0x15, 0x39, 0x33, 0x50, 0x36, 0x13, 0x14, 0x29, 0x2D};
    this->sendData(ST7789_PVGAMCTRL, buf7);

    uint8_t buf8[] = {0xD0, 0x08, 0x10, 0x08, 0x06, 0x06, 0x39, 0x44, 0x51, 0x0B, 0x16, 0x14, 0x2F, 0x31};
    this->sendData(ST7789_NVGAMCTRL, buf8);

    this->sendData(ST7789_INVON);
    this->sendData(ST7789_SLPOUT);
    this->sendData(ST7789_DISPON);

    this->setWindow(0, 0, DISPLAY_WIDTH, DISPLAY_HEIGHT);
    spi_set_format(spi1, 16, SPI_CPOL_0, SPI_CPHA_0, SPI_MSB_FIRST);
}

void Display::reset() {
    gpio_put(CS_PIN, 1);
    sleep_ms(100);
    gpio_put(RST_PIN, 0);
    sleep_ms(100);
    gpio_put(RST_PIN, 1);
    sleep_ms(100);
}

void Display::setWindow(const uint16_t sx, const uint16_t sy, const uint16_t ex, const uint16_t ey){
    uint8_t buf1[] = {sx >> 8, sx & 0xFF, (ex-1) >> 8, (ex-1) & 0xFF};
    this->sendData(ST7789_CASET, buf1);

    uint8_t buf2[] = {sy >> 8, sy & 0xFF, (ey-1) >> 8, (ey-1) & 0xFF};
    this->sendData(ST7789_RASET, buf2);

    this->sendData(ST7789_RAMWR);
}

void Display::setCursor(const uint16_t x, const uint16_t y) {
    this->setWindow(x, y, x+1, y+1);
}

void Display::sendData(const uint8_t cmd, const uint8_t data[]) {    
    this->write_cmd(cmd);
    this->write_data(data);
}

void Display::sendData(const uint8_t cmd, const uint8_t data) {
    this->write_cmd(cmd);
    this->write_data(data);
}

void Display::sendData(const uint8_t cmd) {
    this->write_cmd(cmd);
}

void Display::write_cmd(const uint8_t cmd) {
    gpio_put(DC_PIN, 0);
    uint8_t buf[] = {cmd};
    spi_write_blocking(spi1, buf, 1);
}

void Display::write_data(const uint8_t data) {
    gpio_put(DC_PIN, 1);
    uint8_t buf[] = {data};
    spi_write_blocking(spi1, buf, 1);
}

void Display::write_data(const uint8_t data[]) {
    int n = sizeof(data) / sizeof(data[0]);
    gpio_put(DC_PIN, 1);
    spi_write_blocking(spi1, data, n);
}

void Display::setBrightness(uint8_t brightness) {
    // pwm_set_gpio_level(BL_PIN, int(65535 * brightness / 100));
}

void Display::update() {
    gpio_put(DC_PIN, 1);

    this->frameBuffer->waitOnBusy();
    
    dma_channel_configure(this->dmaSPIChannel, &this->dmaSPIConfig, &spi_get_hw(spi1)->dr, (uint16_t*)this->frameBuffer->buffer, DISPLAY_WIDTH * DISPLAY_HEIGHT, true);
    dma_channel_wait_for_finish_blocking(this->dmaSPIChannel);
}

Display::~Display() {
}

void Display::clear(Color c) {
    this->frameBuffer->clear(c);
}

void Display::setPixel(Vec2 pos, Color &c, uint8_t alpha) {
    this->frameBuffer->setPixel(pos, c, alpha);
}

void Display::drawBitmapRow(Vec2 pos, int width, Color *c) {
    this->frameBuffer->drawBitmapRow(pos, width, c);
}

void Display::fillRect(Rect2 rect, Color &c, uint8_t alpha) {
    this->frameBuffer->fillRect(rect, c, alpha);
}

void Display::hLine(Vec2 pos, int width, Color &c, uint8_t alpha) {
    this->frameBuffer->hLine(pos, width, c, alpha);
}

void Display::vLine(Vec2 pos, int height, Color &c, uint8_t alpha) {
    this->frameBuffer->vLine(pos, height, c, alpha);
}

void Display::rect(Rect2 rect, Color &c, uint8_t alpha) {
    this->frameBuffer->rect(rect, c, alpha);
}
