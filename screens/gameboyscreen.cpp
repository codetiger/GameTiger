#include "gameboyscreen.h"
#define ENABLE_SOUND 0
#define ENABLE_LCD 1
#include "../core/Peanut-GB/peanut_gb.h"
#include "tetrisgb.h"
#include "mariogb.h"

uint8_t gb_rom_read(struct gb_s *gb, const uint_fast32_t addr) {
    GameBoyScreen* p = (GameBoyScreen*)gb->direct.priv;
    return p->rom[addr];
}

uint8_t gb_cart_ram_read(struct gb_s *gb, const uint_fast32_t addr) {
    GameBoyScreen* p = (GameBoyScreen*)gb->direct.priv;
    return p->cart_ram[addr];
}

void gb_cart_ram_write(struct gb_s *gb, const uint_fast32_t addr, const uint8_t val) {
    GameBoyScreen* p = (GameBoyScreen*)gb->direct.priv;
	p->cart_ram[addr] = val;
}

void gb_error(struct gb_s *gb, const enum gb_error_e gb_err, const uint16_t addr) {
	const char* gb_err_str[GB_INVALID_MAX] = {
		"UNKNOWN",
		"INVALID OPCODE",
		"INVALID READ",
		"INVALID WRITE",
		"HALT FOREVER"
	};
	printf("Error %d occurred: %s at %04X\n. Exiting.\n", gb_err, gb_err_str[gb_err], addr);

    GameBoyScreen* p = (GameBoyScreen*)gb->direct.priv;
	/* Free memory and then exit. */
	free(p->cart_ram);
	free(p->rom);
	exit(EXIT_FAILURE);
}

void lcd_draw_line(struct gb_s *gb, const uint8_t pixels[160], const uint_fast8_t line) {
	GameBoyScreen* p = (GameBoyScreen*)gb->direct.priv;
    if(!p || !p->display) {
        // printf("Display Null\n");
        return;
    }

	const Color palette[] = { Color(155, 188, 15), Color(139, 172, 15), Color(48, 98, 48), Color(15, 56, 15) };

    const uint16_t pixel_width = LCD_WIDTH * 1.66f;

    const uint8_t minLine = floor((line * 5.0) / 3.0);
    const uint8_t maxLine = ceil((line * 5.0) / 3.0);

    if(minLine == maxLine) {
        for(uint16_t x = 0; x < pixel_width; x++) {
            uint16_t px = (x * 3) / 5;
            p->display->setPixel(Vec2(27 + x, maxLine), (Color&)palette[pixels[px] & 3], 255);
        }
    } else {
        Color lineColor[pixel_width];
        for(uint16_t x = 0; x < pixel_width; x++) {
            uint16_t px = (x * 3) / 5;
            lineColor[x] = palette[pixels[px] & 3];
        }

        for(uint8_t y = minLine; y <= maxLine; y++)
            p->display->drawBitmapRow(Vec2(27, y), pixel_width, lineColor);
    }
}

GameBoyScreen::GameBoyScreen(void (*rcb)(int8_t menu, uint8_t option), void (*hscb)(uint32_t highscore), uint32_t highscore, uint8_t option) {
    printf("[GameBoyScreen] loading...\n");
    this->screenId = ScreenEnum::GAMEBOYSCREEN;
    this->returnCallBack = rcb;
    this->highScoreCallBack = hscb;
    this->option = option;

    this->rom = (uint8_t*)mario_gb;
    this->gb_ptr = new gb_s();
    this->display = NULL;

    enum gb_init_error_e ret = gb_init((gb_s*)this->gb_ptr, &gb_rom_read, &gb_cart_ram_read, &gb_cart_ram_write, &gb_error, this);
	if(ret != GB_INIT_NO_ERROR) {
		printf("Error: %d\n", ret);
		exit(EXIT_FAILURE);
	}

    this->cart_ram = NULL;
    uint8_t cart_ram_size = gb_get_save_size((gb_s*)this->gb_ptr);
    if(cart_ram_size > 0)
        this->cart_ram = (uint8_t*)malloc(cart_ram_size);

    gb_init_lcd((gb_s*)this->gb_ptr, &lcd_draw_line);
    printf("[GameBoyScreen] Done\n");
}

GameBoyScreen::~GameBoyScreen() {
    this->display = NULL;
    if(this->gb_ptr) {
        free(this->gb_ptr);
        this->gb_ptr = NULL;
    }
	free(this->cart_ram);
	free(this->rom);
}

void GameBoyScreen::update(uint16_t deltaTimeMS) {
    // uint16_t timeDiff = getTimeDiffMS(this->lastUpdate);
    // if(timeDiff > 20) {
    //     this->lastUpdate = getTime();
    // }
    if(this->gb_ptr == NULL)
        return;

    gb_run_frame((gb_s*)this->gb_ptr);
}

void GameBoyScreen::draw(Display *display) {
    if(this->display == NULL) {
        display->clear(BLACKCOLOR);
        this->display = display;
    }
}

void GameBoyScreen::keyPressed(uint8_t key) {
    if(key == KEY_EXIT)
        ((gb_s*)this->gb_ptr)->direct.joypad_bits.start = 0;
    else if(key == KEY_UP)
        ((gb_s*)this->gb_ptr)->direct.joypad_bits.up = 0;
    else if(key == KEY_DOWN)
        ((gb_s*)this->gb_ptr)->direct.joypad_bits.down = 0;
    else if(key == KEY_LEFT)
        ((gb_s*)this->gb_ptr)->direct.joypad_bits.left = 0;
    else if(key == KEY_RIGHT)
        ((gb_s*)this->gb_ptr)->direct.joypad_bits.right = 0;
    else if(key == KEY_A)
        ((gb_s*)this->gb_ptr)->direct.joypad_bits.a = 0;
    else if(key == KEY_B)
        ((gb_s*)this->gb_ptr)->direct.joypad_bits.b = 0;
}

void GameBoyScreen::keyReleased(uint8_t key) {
    if(key == KEY_EXIT)
        ((gb_s*)this->gb_ptr)->direct.joypad_bits.start = 1;
    else if(key == KEY_UP)
        ((gb_s*)this->gb_ptr)->direct.joypad_bits.up = 1;
    else if(key == KEY_DOWN)
        ((gb_s*)this->gb_ptr)->direct.joypad_bits.down = 1;
    else if(key == KEY_LEFT)
        ((gb_s*)this->gb_ptr)->direct.joypad_bits.left = 1;
    else if(key == KEY_RIGHT)
        ((gb_s*)this->gb_ptr)->direct.joypad_bits.right = 1;
    else if(key == KEY_A)
        ((gb_s*)this->gb_ptr)->direct.joypad_bits.a = 1;
    else if(key == KEY_B)
        ((gb_s*)this->gb_ptr)->direct.joypad_bits.b = 1;
}

void GameBoyScreen::keyDown(uint8_t key) {
}
