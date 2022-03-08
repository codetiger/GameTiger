#include "display.h"
#include <stdio.h>
#include <algorithm>

Display::Display() {
    this->initHardware();
    this->initSequence();
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
    
    this->sendData(0x36, (uint8_t)0x78);
    this->sendData(0x3A, 0x05);

    this->write_cmd(0x21);

    uint8_t buf1[] = {0x00, 0x00, 0x01, 0x3F};
    this->sendData(0x2A, buf1);

    uint8_t buf2[] = {0x00, 0x00, 0x00, 0xEF};
    this->sendData(0x2B, buf2);

    uint8_t buf3[] = {0x00, 0xE8};
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
    for (int i = 0; i < this->height; i++)
        for (int j = 0; j < this->width; j++)
            this->setPixel(j, i, c);
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

void Display::setPixel(int x, int y, Color c, uint8_t alpha) {
    if (x < 0 || x >= this->width || y < 0 || y >= this->height)
        return;
    
    int index = (y * this->width) + x;
    if(alpha != 255) {
        this->buffer[index].red = (c.red * alpha + this->buffer[index].red * (255 - alpha)) / 255;
        this->buffer[index].green = (c.green * alpha + this->buffer[index].green * (255 - alpha)) / 255;
        this->buffer[index].blue = (c.blue * alpha + this->buffer[index].blue * (255 - alpha)) / 255;
    } else
        this->buffer[index] = c;
}

void Display::update() {
    this->setWindow(0, 0, this->width, this->height);
    gpio_put(DC_PIN, 1);
    spi_write_blocking(spi1, (uint8_t*)this->buffer, this->width * this->height * 2);
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
    gpio_put(CS_PIN, 0);
    gpio_put(DC_PIN, 0);
    uint8_t buf[] = {cmd};
    spi_write_blocking(spi1, buf, 1);
}

void Display::write_data(const uint8_t data) {
    gpio_put(CS_PIN, 0);
    gpio_put(DC_PIN, 1);
    uint8_t buf[] = {data};
    spi_write_blocking(spi1, buf, 1);
    gpio_put(CS_PIN, 1);
}

void Display::write_data(const uint8_t data[]) {
    int n = sizeof(data) / sizeof(data[0]);
    for (int i = 0; i < n; i++)
        this->write_data(data[i]);
}

void Display::fillRect(int x, int y, int width, int height, Color c, uint8_t alpha) {
    if(x >= this->width || y >= this->height || x + width < 0 || y + height < 0)
        return;

    for (int i = y; i < y + height; i++)
        for (int j = x; j < x + width; j++)
            this->setPixel(j, i, c, alpha);
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
