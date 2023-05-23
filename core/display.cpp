#include "display.h"
#include <stdio.h>
#include <algorithm>
#include <memory>

Display::Display() {
    printf("[Display] driver loading...\n");
    this->buffer = new Color[DISPLAY_WIDTH * DISPLAY_HEIGHT];
#ifdef FORMPU
    this->initHardware();
    this->initSequence();
#else
    if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
		printf("[Display] SDL_Init Error: %s\n", SDL_GetError());
        exit(EXIT_FAILURE);
	}

	this->window = SDL_CreateWindow("GameTiger!", 100, 100, DISPLAY_WIDTH, DISPLAY_HEIGHT, SDL_WINDOW_SHOWN);
	if (this->window == NULL) {
		printf("[Display] SDL_CreateWindow Error: %s\n", SDL_GetError());
        exit(EXIT_FAILURE);
	}

	this->renderer = SDL_CreateRenderer(this->window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	if (this->renderer == NULL) {
		printf("[Display] SDL_CreateRenderer Error: %s\n", SDL_GetError());
		SDL_DestroyWindow(this->window);
		SDL_Quit();
        exit(EXIT_FAILURE);
	}
    SDL_SetRenderDrawColor(this->renderer, 255, 255, 255, 255);
    SDL_RenderClear(this->renderer);
#endif
    printf("[Display] Done\n");
}

#ifdef FORMPU
void Display::initHardware() {
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

    this->dmaBufferChannel = dma_claim_unused_channel(true);
    this->dmaBufferConfig = dma_channel_get_default_config(this->dmaBufferChannel);
    channel_config_set_transfer_data_size(&this->dmaBufferConfig, DMA_SIZE_16);
    channel_config_set_read_increment(&this->dmaBufferConfig, true);
    channel_config_set_write_increment(&this->dmaBufferConfig, true);
    channel_config_set_ring(&this->dmaBufferConfig, false, 0);

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
    this->setBrightness(100);

    int br = spi_init(spi1, 62.5 * 1000 * 1000); //62.5
    // printf("Display baudrate: %d\n", br);

    gpio_set_function(SCK_PIN, GPIO_FUNC_SPI);
    gpio_set_function(MOSI_PIN, GPIO_FUNC_SPI);
}

void Display::initSequence() {
    this->reset();
    gpio_put(CS_PIN, 0);

    this->sendData(0x36, (uint8_t)0xB8);  //0xB8 or 0x78 rotation
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
    this->sendData(0x2A, buf1);

    uint8_t buf2[] = {sy >> 8, sy & 0xFF, (ey-1) >> 8, (ey-1) & 0xFF};
    this->sendData(0x2B, buf2);

    this->write_cmd(0x2C);
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
#endif

void Display::setBrightness(uint8_t brightness) {
#ifdef FORMPU
    pwm_set_gpio_level(BL_PIN, int(65535 * brightness / 100));
#endif
}

void Display::clear(Color c) {
#ifdef FORMPU
    dma_channel_configure(this->dmaMemChannel, &this->dmaMemConfig, this->buffer, &c, DISPLAY_HEIGHT*DISPLAY_WIDTH, true);
    dma_channel_wait_for_finish_blocking(this->dmaMemChannel);
#else
    for (int i = 0; i < DISPLAY_HEIGHT; i++) {
        for (int j = 0; j < DISPLAY_WIDTH; j++) {
            int index = (i * DISPLAY_WIDTH) + j;
            this->buffer[index] = c;
        }
    }
#endif
}

#define div_255_fast(x) (((x) + (((x) + 257) >> 8)) >> 8)

void Display::setPixel(Vec2 pos, Color &c, uint8_t alpha) {
    if (alpha == 0 || pos.x < 0 || pos.x >= DISPLAY_WIDTH || pos.y < 0 || pos.y >= DISPLAY_HEIGHT)
        return;
    
    int index = (pos.y * DISPLAY_WIDTH) + pos.x;
    if(alpha > 250) {
        this->buffer[index] = c;
    } else {
        uint8_t ralpha = 255 - alpha;

        this->buffer[index].Colors.red = div_255_fast(c.Colors.red * alpha + this->buffer[index].Colors.red * ralpha);
        this->buffer[index].Colors.green = div_255_fast(c.Colors.green * alpha + this->buffer[index].Colors.green * ralpha);
        this->buffer[index].Colors.blue = div_255_fast(c.Colors.blue * alpha + this->buffer[index].Colors.blue * ralpha);
    }
}

void Display::drawBitmapRow(Vec2 pos, int width, Color *c) {
#ifdef FORMPU
    int index = (pos.y * DISPLAY_WIDTH) + pos.x;
    dma_channel_configure(this->dmaBufferChannel, &this->dmaBufferConfig, &this->buffer[index], c, width, true);
    dma_channel_wait_for_finish_blocking(this->dmaBufferChannel);
#else
    for (int i = 0; i < width; i++)
        this->setPixel(Vec2(pos.x+i, pos.y), c[i], 255);
#endif
}

void Display::update() {
#ifdef FORMPU
    gpio_put(DC_PIN, 1);

    dma_channel_configure(this->dmaSPIChannel, &this->dmaSPIConfig, &spi_get_hw(spi1)->dr, (uint16_t*)this->buffer, DISPLAY_WIDTH * DISPLAY_HEIGHT, true);
    dma_channel_wait_for_finish_blocking(this->dmaSPIChannel);
#else
    for (int i = 0; i < DISPLAY_HEIGHT; i++) {
        for (int j = 0; j < DISPLAY_WIDTH; j++) {
            int index = (i * DISPLAY_WIDTH) + j;
            SDL_SetRenderDrawColor(this->renderer, this->buffer[index].Colors.red<<3, this->buffer[index].Colors.green<<2, this->buffer[index].Colors.blue<<3, 255);
            SDL_RenderDrawPoint(this->renderer, j, i);
        }
    }
    SDL_RenderPresent(this->renderer);
#endif
}

void Display::fillRect(Rect2 rect, Color &c, uint8_t alpha) {
    if(rect.x >= DISPLAY_WIDTH || rect.y >= DISPLAY_HEIGHT || rect.x + rect.w < 0 || rect.y + rect.h < 0)
        return;

    rect.w = std::min(rect.w, (Index)(DISPLAY_WIDTH - rect.x));
    rect.h = std::min(rect.h, (Index)(DISPLAY_HEIGHT - rect.y));
    rect.x = std::max(rect.x, (Unit)0);
    rect.y = std::max(rect.y, (Unit)0);

    if(alpha != 255 || rect.w < 8) {
        for (int i = rect.y; i < rect.y + rect.h; i++)
            for (int j = rect.x; j < rect.x + rect.w; j++)
                this->setPixel(Vec2(j, i), c, alpha);
    } else {
        #ifdef FORMPU
        for (int i = rect.y; i < rect.y + rect.h; i++) {
            int index = (i * DISPLAY_WIDTH) + rect.x;
            dma_channel_configure(this->dmaMemChannel, &this->dmaMemConfig, &(this->buffer)[index], &c, rect.w, true);
            dma_channel_wait_for_finish_blocking(this->dmaMemChannel);
        }
        #else
        for (int i = rect.y; i < rect.y + rect.h; i++)
            for (int j = rect.x; j < rect.x + rect.w; j++)
                this->setPixel(Vec2(j, i), c, alpha);
        #endif
    }
}

Display::~Display() {
}

void Display::hLine(Vec2 pos, int width, Color &c, uint8_t alpha) {
    this->fillRect(Rect2(pos.x, pos.y, width, 1), c, alpha);
}

void Display::vLine(Vec2 pos, int height, Color &c, uint8_t alpha) {
    this->fillRect(Rect2(pos.x, pos.y, 1, height), c, alpha);
}

void Display::rect(Rect2 rect, Color &c, uint8_t alpha) {
    this->hLine(Vec2(rect.x, rect.y), rect.w, c, alpha);
    this->hLine(Vec2(rect.x, rect.y + rect.h), rect.w, c, alpha);
    this->vLine(Vec2(rect.x, rect.y), rect.h, c, alpha);
    this->vLine(Vec2(rect.x + rect.w, rect.y), rect.h, c, alpha);
}

void Display::line(Vec2 p0, Vec2 p1, Color &c, uint8_t alpha) {
   	bool yLonger = false;
	int shortLen = p1.y - p0.y;
	int longLen = p1.x - p0.x;
	if(abs(shortLen) > abs(longLen)) {
		int temp = shortLen;
		shortLen = longLen;
		longLen = temp;
		yLonger = true;
	}

	int decInc = (longLen == 0) ? 0 : ((shortLen << 16) / longLen);
	if (yLonger) {
		if (longLen > 0) {
			longLen += p0.y;
			for (int j = 0x8000 + (p0.x<<16); p0.y <= longLen; ++p0.y) {
                this->setPixel(Vec2(j >> 16, p0.y), c, alpha);
				j += decInc;
			}
			return;
		}
		longLen += p0.y;
		for (int j = 0x8000 + (p0.x<<16); p0.y >= longLen; --p0.y) {
            this->setPixel(Vec2(j >> 16, p0.y), c, alpha);
			j -= decInc;
		}
		return;	
	}

	if (longLen > 0) {
		longLen += p0.x;
		for (int j = 0x8000 + (p0.y<<16); p0.x <= longLen; ++p0.x) {
            this->setPixel(Vec2(p0.x, j >> 16), c, alpha);
			j += decInc;
		}
		return;
	}
	longLen += p0.x;
	for (int j = 0x8000 + (p0.y<<16); p0.x >= longLen; --p0.x) {
        this->setPixel(Vec2(p0.x, j >> 16), c, alpha);
		j -= decInc;
	}
}

void Display::triangle(Vec2 p0, Vec2 p1, Vec2 p2, Color &c, uint8_t alpha) {
    this->line(p0, p1, c, alpha);
    this->line(p1, p2, c, alpha);
    this->line(p2, p0, c, alpha);
}

void Display::fillTriangle(Vec2 p0, Vec2 p1, Vec2 p2, Color &c, uint8_t alpha) {
    if(p0.y == p1.y && p0.y == p2.y) return;
    if(p0.y > p1.y) std::swap(p0, p1);
    if(p0.y > p2.y) std::swap(p0, p2);
    if(p1.y > p2.y) std::swap(p1, p2);

    Unit total_height = p2.y - p0.y;
    for (Unit i = 0; i < total_height; i++) { 
        bool second_half = i > p1.y-p0.y || p1.y == p0.y; 
        Unit segment_height = second_half ? p2.y-p1.y : p1.y-p0.y;
        Vec2 A = p0 + ((p2-p0) * i) / total_height;
        Vec2 B; 
        if(second_half)
            B = p1 + ((p2-p1) * (i-p1.y+p0.y)) / segment_height;
        else
            B = p0 + ((p1-p0) * i) / segment_height;

        if (A.x > B.x) std::swap(A, B); 
        this->hLine(Vec2(A.x, p0.y+i), B.x - A.x, c, alpha);
    } 
}
