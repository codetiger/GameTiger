#include "gameboyscreen.h"
#define ENABLE_SOUND 0
#define ENABLE_LCD 1
#include "../core/Peanut-GB/peanut_gb.h"
#include "gbgames/tetrisgb.h"
#include "gbgames/mariogb.h"
#include "gbgames/kirby.h"
#include "gbgames/metroid2.h"
#include "gbgames/tetris2.h"

void gb_load_palette(struct gb_s *gb) {
    GameBoyScreen* p = (GameBoyScreen*)gb->direct.priv;

    uint8_t checksum = gb_colour_hash(gb);
    printf("Game Hash: %X\n", checksum);
    p->palette = new Color[3 * 4];

    switch(checksum) {
    	/* Balloon Kid (USA, Europe) */
        /* Tetris Blast (USA, Europe) */
        case 0x71:
        case 0xFF: {
            const uint32_t palette[3][4] = {
                { 0xFFFFFF, 0xFF9C00, 0xFF0000, 0x000000 }, /* OBJ0 */
                { 0xFFFFFF, 0xFF9C00, 0xFF0000, 0x000000 }, /* OBJ1 */
                { 0xFFFFFF, 0xFF9C00, 0xFF0000, 0x000000 }  /* BG */
            };
            for(uint8_t obj = 0; obj < 3; obj++)
                for(uint8_t layer = 0; layer < 4; layer++)
                    p->palette[(obj * 4) + layer] = Color(palette[obj][layer]);
            break;
        }

        /* Hoshi no Kirby */
        /* Kirby no Block Ball */
        /* Kirby's Block Ball */
        /* Kirby's Dream Land */
        case 0x27:
        case 0x49:
        case 0x5C:
        case 0xB3: {
            const uint32_t palette[3][4] = {
                { 0xFF6352, 0xD60000, 0x630000, 0x000000 }, /* OBJ0 */
                { 0x0000FF, 0xFFFFFF, 0xFFFF7B, 0x0084FF }, /* OBJ1 */
                { 0xA59CFF, 0xFFFF00, 0x006300, 0x000000 }  /* BG */
            };
            for(uint8_t obj = 0; obj < 3; obj++)
                for(uint8_t layer = 0; layer < 4; layer++)
                    p->palette[(obj * 4) + layer] = Color(palette[obj][layer]);
            break;
        }

        /* Qix */
        /* Tetris 2 */
        /* Tetris Flash */
        case 0x0D:
        case 0x69:
        case 0xF2: {
            const uint32_t palette[3][4] = {
                { 0xFFFFFF, 0xFFFF00, 0xFF0000, 0x000000 }, /* OBJ0 */
                { 0xFFFFFF, 0xFFFF00, 0xFF0000, 0x000000 }, /* OBJ1 */
                { 0xFFFFFF, 0x5ABDFF, 0xFF0000, 0x0000FF }  /* BG */
            };
            for(uint8_t obj = 0; obj < 3; obj++)
                for(uint8_t layer = 0; layer < 4; layer++)
                    p->palette[(obj * 4) + layer] = Color(palette[obj][layer]);
            break;
        }

        /* Pocket Monsters - Pikachu */
        /* Tetris */
        case 0x15:
        case 0xDB: {
            const uint32_t palette[3][4] = {
                { 0xFFFFFF, 0xFFFF00, 0xFF0000, 0x000000 }, /* OBJ0 */
                { 0xFFFFFF, 0xFFFF00, 0xFF0000, 0x000000 }, /* OBJ1 */
                { 0xFFFFFF, 0xFFFF00, 0xFF0000, 0x000000 }  /* BG */
            };
            for(uint8_t obj = 0; obj < 3; obj++)
                for(uint8_t layer = 0; layer < 4; layer++)
                    p->palette[(obj * 4) + layer] = Color(palette[obj][layer]);
            break;
        }

        /* Super Mario Land 2 */
        case 0xC9: {
            const uint32_t palette[3][4] = {
                { 0xFFFFFF, 0xFF7300, 0x944200, 0x000000 }, /* OBJ0 */
                { 0xFFFFFF, 0x63A5FF, 0x0000FF, 0x000000 }, /* OBJ1 */
                { 0xFFFFCE, 0x63EFEF, 0x9C8431, 0x5A5A5A }  /* BG */
            };
            for(uint8_t obj = 0; obj < 3; obj++)
                for(uint8_t layer = 0; layer < 4; layer++)
                    p->palette[(obj * 4) + layer] = Color(palette[obj][layer]);
            break;
        }

        /* Metroid II - Return of Samus  */
        case 0x46: {
            const uint32_t palette[3][4] = {
                { 0xFFFF00, 0xFF0000, 0x630000, 0x000000 }, /* OBJ0 */
                { 0xFFFFFF, 0x7BFF31, 0x008400, 0x000000 }, /* OBJ1 */
                { 0xFFFFFF, 0x63A5FF, 0x0000FF, 0x000000 }  /* BG */
            };
            for(uint8_t obj = 0; obj < 3; obj++)
                for(uint8_t layer = 0; layer < 4; layer++)
                    p->palette[(obj * 4) + layer] = Color(palette[obj][layer]);
            break;
        }

        default: {
            const uint32_t palette[3][4] = {
                { 0xFFFFFF, 0x7BFF31, 0x008400, 0x000000 }, /* OBJ0 */
                { 0xFFFFFF, 0x7BFF31, 0x008400, 0x000000 }, /* OBJ1 */
                { 0xFFFFFF, 0x7BFF31, 0x008400, 0x000000 }  /* BG */
            };
            for(uint8_t obj = 0; obj < 3; obj++)
                for(uint8_t layer = 0; layer < 4; layer++)
                    p->palette[(obj * 4) + layer] = Color(palette[obj][layer]);
            break;
        }
    }
}

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

    const uint16_t pixel_width = LCD_WIDTH * 1.66f;
    const uint8_t minLine = floor((line * 5.0) / 3.0);
    const uint8_t maxLine = ceil((line * 5.0) / 3.0);

    if(minLine == maxLine) {
        for(uint16_t x = 0; x < pixel_width; x++) {
            uint16_t px = (x * 3) / 5;
            uint8_t obj = (pixels[px] & 0x30) >> 4;
            uint8_t layer = pixels[px] & 3;
            Color c = p->palette[(obj * 4) + layer];
            p->display->setPixel(Vec2(27 + x, maxLine), c, 255);
        }
    } else {
        Color lineColor[pixel_width];
        for(uint16_t x = 0; x < pixel_width; x++) {
            uint16_t px = (x * 3) / 5;
            uint8_t obj = (pixels[px] & 0x30) >> 4;
            uint8_t layer = pixels[px] & 3;
            lineColor[x] = p->palette[(obj * 4) + layer];
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

    if(option == 1)
        this->rom = (uint8_t*)mario_gb;
    else if(option == 2)
        this->rom = (uint8_t*)kirby_gb;
    else if(option == 3)
        this->rom = (uint8_t*)metroid2_gb;
    this->gb_ptr = new gb_s();
    this->display = NULL;

    enum gb_init_error_e ret = gb_init((gb_s*)this->gb_ptr, &gb_rom_read, &gb_cart_ram_read, &gb_cart_ram_write, &gb_error, this);
	if(ret != GB_INIT_NO_ERROR) {
		printf("Error: %d\n", ret);
		exit(EXIT_FAILURE);
	}

    this->cart_ram = NULL;
    uint8_t cart_ram_size = gb_get_save_size((gb_s*)this->gb_ptr);
    printf("[GameBoyScreen] cart_ram_size: %d\n", cart_ram_size);
    if(cart_ram_size > 0)
        this->cart_ram = (uint8_t*)malloc(cart_ram_size);

    gb_init_lcd((gb_s*)this->gb_ptr, &lcd_draw_line);
    gb_load_palette((gb_s*)this->gb_ptr);
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
