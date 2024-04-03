#include "framebuffer.h"

FrameBuffer::FrameBuffer(uint16_t width, uint16_t height) {
    printf("[FrameBuffer] loading...\n");
    this->buffer = new Color[width * height];

    this->dmaCopyChannel = dma_claim_unused_channel(true);
    this->dmaCopyConfig = dma_channel_get_default_config(this->dmaCopyChannel);
    channel_config_set_transfer_data_size(&this->dmaCopyConfig, DMA_SIZE_16);
    channel_config_set_read_increment(&this->dmaCopyConfig, true);
    channel_config_set_write_increment(&this->dmaCopyConfig, true);
    channel_config_set_ring(&this->dmaCopyConfig, false, 0);

    this->dmaFillChannel = dma_claim_unused_channel(true);
    this->dmaFillConfig = dma_channel_get_default_config(this->dmaFillChannel);
    channel_config_set_transfer_data_size(&this->dmaFillConfig, DMA_SIZE_16);
    channel_config_set_read_increment(&this->dmaFillConfig, false);
    channel_config_set_write_increment(&this->dmaFillConfig, true);
    channel_config_set_ring(&this->dmaFillConfig, false, 0);

    printf("[FrameBuffer] Done\n");
}

FrameBuffer::~FrameBuffer() {
}

void FrameBuffer::clear(Color c) {
    dma_channel_configure(this->dmaFillChannel, &this->dmaFillConfig, this->buffer, &c, DISPLAY_HEIGHT*DISPLAY_WIDTH, true);
    dma_channel_wait_for_finish_blocking(this->dmaFillChannel);
}

void FrameBuffer::setPixel(Vec2 pos, Color &c, uint8_t alpha) {
    if (alpha == 0 || pos.x < 0 || pos.x >= DISPLAY_WIDTH || pos.y < 0 || pos.y >= DISPLAY_HEIGHT)
        return;

    int index = (pos.y * DISPLAY_WIDTH) + pos.x;
    if(alpha > 250)
        this->buffer[index] = c;
    else
        this->buffer[index].mix(c, alpha);
}

void FrameBuffer::drawBitmapRow(Vec2 pos, int width, Color *c) {
    int index = (pos.y * DISPLAY_WIDTH) + pos.x;
    dma_channel_configure(this->dmaCopyChannel, &this->dmaCopyConfig, &this->buffer[index], c, width, true);
    dma_channel_wait_for_finish_blocking(this->dmaCopyChannel);
}

void FrameBuffer::fillRect(Rect2 rect, Color &c, uint8_t alpha) {
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
        for (int i = rect.y; i < rect.y + rect.h; i++) {
            int index = (i * DISPLAY_WIDTH) + rect.x;
            dma_channel_configure(this->dmaFillChannel, &this->dmaFillConfig, &(this->buffer)[index], &c, rect.w, true);
            dma_channel_wait_for_finish_blocking(this->dmaFillChannel);
        }
    }
}

void FrameBuffer::hLine(Vec2 pos, int width, Color &c, uint8_t alpha) {
    this->fillRect(Rect2(pos.x, pos.y, width, 1), c, alpha);
}

void FrameBuffer::vLine(Vec2 pos, int height, Color &c, uint8_t alpha) {
    this->fillRect(Rect2(pos.x, pos.y, 1, height), c, alpha);
}

void FrameBuffer::rect(Rect2 rect, Color &c, uint8_t alpha) {
    this->hLine(Vec2(rect.x, rect.y), rect.w, c, alpha);
    this->hLine(Vec2(rect.x, rect.y + rect.h), rect.w, c, alpha);
    this->vLine(Vec2(rect.x, rect.y), rect.h, c, alpha);
    this->vLine(Vec2(rect.x + rect.w, rect.y), rect.h, c, alpha);
}

void FrameBuffer::line(Vec2 p0, Vec2 p1, Color &c, uint8_t alpha) {
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

void FrameBuffer::triangle(Vec2 p0, Vec2 p1, Vec2 p2, Color &c, uint8_t alpha) {
    this->line(p0, p1, c, alpha);
    this->line(p1, p2, c, alpha);
    this->line(p2, p0, c, alpha);
}

void FrameBuffer::fillTriangle(Vec2 p0, Vec2 p1, Vec2 p2, Color &c, uint8_t alpha) {
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
