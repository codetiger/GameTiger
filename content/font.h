#ifdef FORMPU
    #include "pico/stdlib.h"
#endif
#include <map>
#include <array>

extern uint16_t font_img_width;
extern uint16_t font_img_height;
extern uint16_t font_color_count;

extern const uint8_t font_palette[];
extern const uint8_t font_pixel_data[];
extern std::map<char, std::array<uint8_t, 5> > font_sprite_data;