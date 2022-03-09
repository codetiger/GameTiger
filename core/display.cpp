#include "display.h"
#include <stdio.h>
#include <algorithm>

Display::Display() {
    this->initHardware();
    this->initSequence();

    this->dmaSPIChannel = dma_claim_unused_channel(true);
    this->dmaSPIConfig = dma_channel_get_default_config(this->dmaSPIChannel);
    channel_config_set_transfer_data_size(&this->dmaSPIConfig, DMA_SIZE_16);
    channel_config_set_read_increment(&this->dmaSPIConfig, true);
    channel_config_set_write_increment(&this->dmaSPIConfig, false);
    channel_config_set_ring(&this->dmaSPIConfig, false, 0);
    channel_config_set_dreq(&this->dmaSPIConfig, DREQ_SPI1_TX);

    this->dmaMemChannel = dma_claim_unused_channel(true);
    this->dmaMemConfig = dma_channel_get_default_config(this->dmaMemChannel);
    channel_config_set_transfer_data_size(&this->dmaMemConfig, DMA_SIZE_16);
    channel_config_set_read_increment(&this->dmaMemConfig, false);
    channel_config_set_write_increment(&this->dmaMemConfig, true);
    channel_config_set_ring(&this->dmaMemConfig, false, 0);
}

void Display::initHardware() {
    gpio_init(CS_PIN);
    gpio_set_dir(CS_PIN, GPIO_OUT);

    gpio_init(RST_PIN);
    gpio_set_dir(RST_PIN, GPIO_OUT);

    gpio_init(DC_PIN);
    gpio_set_dir(DC_PIN, GPIO_OUT);

    gpio_set_function(BL_PIN, GPIO_FUNC_PWM);
    uint slice_num = pwm_gpio_to_slice_num(BL_PIN);
    pwm_config config = pwm_get_default_config();
    pwm_config_set_clkdiv(&config, 4.f);
    pwm_init(slice_num, &config, true);
    this->setBrightness(50);

    int br = spi_init(spi1, 62.5 * 1000 * 1000);
    printf("Display baudrate: %d\n", br);

    gpio_set_function(SCK_PIN, GPIO_FUNC_SPI);
    gpio_set_function(MOSI_PIN, GPIO_FUNC_SPI);
}

void Display::initSequence() {
    this->reset();
    gpio_put(CS_PIN, 0);

    this->sendData(0x36, (uint8_t)0x78);
    this->sendData(0x3A, 0x05);

    this->write_cmd(0x21);

    uint8_t buf1[] = {0x00, 0x00, 0x01, 0x3F};
    this->sendData(0x2A, buf1);

    uint8_t buf2[] = {0x00, 0x00, 0x00, 0xEF};
    this->sendData(0x2B, buf2);

    uint8_t buf3[] = {0x00, 0xE0};
    this->sendData(0xB0, buf3);

    uint8_t buf4[] = {0x0C, 0x0C, 0x00, 0x33, 0x33};
    this->sendData(0xB2, buf4);

    this->sendData(0xB7, 0x35);
    this->sendData(0xBB, 0x1F);
    this->sendData(0xC0, 0x2C);
    this->sendData(0xC2, 0x01);
    this->sendData(0xC3, 0x12);
    this->sendData(0xC4, 0x20);
    this->sendData(0xC6, 0x0F);

    uint8_t buf5[] = {0xA4, 0xA1};
    this->sendData(0xD0, buf5);

    uint8_t buf6[] = {0xD0, 0x08, 0x11, 0x08, 0x0C, 0x15, 0x39, 0x33, 0x50, 0x36, 0x13, 0x14, 0x29, 0x2D};
    this->sendData(0xE0, buf6);

    uint8_t buf7[] = {0xD0, 0x08, 0x10, 0x08, 0x06, 0x06, 0x39, 0x44, 0x51, 0x0B, 0x16, 0x14, 0x2F, 0x31};
    this->sendData(0xE1, buf7);

    this->write_cmd(0x21);
    this->write_cmd(0x11);
    this->write_cmd(0x29);

    this->setWindow(0, 0, this->width, this->height);
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

void Display::setBrightness(uint8_t brightness) {
    pwm_set_gpio_level(BL_PIN, int(65535 * brightness / 100));
}

void Display::clear(Color c) {
    dma_channel_configure(this->dmaMemChannel, &this->dmaMemConfig, &this->buffer, &c, this->height*this->width, true);
    dma_channel_wait_for_finish_blocking(this->dmaMemChannel);
}

void Display::setWindow(const uint16_t sx, const uint16_t sy, const uint16_t ex, const uint16_t ey){
    uint8_t buf1[] = {sx >> 8, sx & 0xFF, (ex-1) >> 8, (ex-1) & 0xFF};
    this->sendData(0x2A, buf1);

    uint8_t buf2[] = {sy >> 8, sy & 0xFF, (ey-1) >> 8, (ey-1) & 0xFF};
    this->sendData(0x2B, buf2);

    this->write_cmd(0x2C);
}

void Display::setCursor(const uint16_t x, const uint16_t y) {
    this->setWindow(x, y, x+1, y+1);
}

#define div_255_fast(x)    (((x) + (((x) + 257) >> 8)) >> 8)

void Display::setPixel(int x, int y, Color c, uint8_t alpha) {
    if (x < 0 || x >= this->width || y < 0 || y >= this->height)
        return;
    
    int index = (y * this->width) + x;
    alpha = (alpha & 0xc0) | (alpha | 0x3f);
    if(alpha > 192) {
        this->buffer[index] = c;
    } else if(alpha < 64) {
    } else {
        uint8_t ralpha = 255 - alpha;
        this->buffer[index].red = div_255_fast(c.red * alpha + this->buffer[index].red * ralpha);
        this->buffer[index].green = div_255_fast(c.green * alpha + this->buffer[index].green * ralpha);
        this->buffer[index].blue = div_255_fast(c.blue * alpha + this->buffer[index].blue * ralpha);
    }
}

void Display::update() {
    gpio_put(DC_PIN, 1);

    dma_channel_configure(this->dmaSPIChannel, &this->dmaSPIConfig, &spi_get_hw(spi1)->dr, (uint16_t*)this->buffer, this->width * this->height, true);
    dma_channel_wait_for_finish_blocking(this->dmaSPIChannel);
}

void Display::sendData(const uint8_t cmd, const uint8_t data[]) {
    this->write_cmd(cmd);
    this->write_data(data);
}

void Display::sendData(const uint8_t cmd, const uint8_t data) {
    this->write_cmd(cmd);
    this->write_data(data);
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

void Display::fillRect(int x, int y, int width, int height, Color c, uint8_t alpha) {
    if(x >= this->width || y >= this->height || x + width < 0 || y + height < 0)
        return;

    if(alpha != 255 || width < 8) {
        for (int i = y; i < y + height; i++)
            for (int j = x; j < x + width; j++)
                this->setPixel(j, i, c, alpha);
    } else {
        width = std::min(width, this->width - x);
        height = std::min(height, this->height - y);
        x = std::max(x, 0);
        y = std::max(y, 0);
        
        for (int i = y; i < y + height; i++) {
            int index = (i * this->width) + x;
            dma_channel_configure(this->dmaMemChannel, &this->dmaMemConfig, &this->buffer[index], &c, width, true);
            dma_channel_wait_for_finish_blocking(this->dmaMemChannel);
        }
    }
}

Display::~Display() {
}

void Display::hLine(int x, int y, int width, Color c, uint8_t alpha) {
    this->fillRect(x, y, width, 1, c, alpha);
}

void Display::vLine(int x, int y, int height, Color c, uint8_t alpha) {
    this->fillRect(x, y, 1, height, c, alpha);
}

void Display::rect(int x, int y, int width, int height, Color c, uint8_t alpha) {
    this->hLine(x, y, width, c, alpha);
    this->hLine(x, y + height, width, c, alpha);
    this->vLine(x, y, height, c, alpha);
    this->vLine(x + width, y, height, c, alpha);
}
