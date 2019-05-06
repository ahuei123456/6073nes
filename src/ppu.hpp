#ifndef ppu_hpp
#define ppu_hpp

#include <cstdint>
#include <array>
#include <exception>
#include <memory>
#include <SDL.h>
#include "mem.hpp"

#define SPRITES 0x40
#define SPRITES_SEC 8
#define LEFT 0xFFF
#define RIGHT 0xFFF
#define REGS 8

class Mem;

struct Sprite{
    uint8_t Y;
    uint8_t index;
    uint8_t attributes;
    uint8_t X;
    
    uint8_t palette();
    uint8_t priority();
    uint8_t horizontal_flip();
    uint8_t vertical_flip();
};

class PPU {
private:
    //Pointer to overall memory
    std::shared_ptr<Mem> memory;
    //PPU registers
    std::array<uint8_t, REGS> regs;
    //OAM memory
    std::array<Sprite, SPRITES> oam;
    std::array<Sprite, SPRITES_SEC> oam_sec;
    
    uint8_t read_buffer;
    uint8_t primary_oam_byte;
    uint8_t n = 0;
    uint8_t m = 0;
    uint8_t oam_sec_index = 0;
    bool oam_sec_full = false;
    bool in_range = false;

    //PPU memory used for rendering specified in documentation
    uint16_t vram_addr;
    uint16_t temp_vram_addr;
    std::array<uint8_t, SPRITES_SEC> sprite_bitmap_low;
    std::array<uint8_t, SPRITES_SEC> sprite_bitmap_high;
    std::array<uint8_t, SPRITES_SEC> sprite_attributes;
    std::array<uint8_t, SPRITES_SEC> sprite_x;
    uint8_t attribute_byte_1, attribute_byte_2;
    uint16_t background_bitmap_1, background_bitmap_2;
    //Positions for upper left corner of screen
    uint8_t scroll_x;
    uint8_t scroll_y;
    uint8_t fine_x;

    //Determines whether x or y coordinate is set next. If false, x-coordinate. If true, y-coordinate.
    bool addr_latch = false;
    uint16_t bitmap_latch;
    uint8_t sprite_x_latch;
    uint8_t sprite_attribute_latch;
    uint8_t sprite_tile_latch;
    uint8_t sprite_y_latch;

    //Variables used to keep track of where we are in cycles/scanlines.
    uint8_t current_scanline = -1;
    uint8_t cycle_mod_8 = 0;
    uint8_t cycle_mod_341 = 0;

    uint8_t nametable_byte;
    bool sprite_foreground;

    std::array<std::array<uint8_t, 256>, 240> pixel_array;

public:
    PPU(std::shared_ptr<Mem> memory);
    void set_reg(uint64_t index, uint8_t value);
   
    void set_oam(uint8_t byte);

    void set_vram_addr(uint8_t value);

    uint16_t get_vram_addr();

    void inc_vram_addr(uint8_t type);

    void set_scroll_coord(uint8_t value);

    void update_buffer();

    uint8_t get_buffer();

    void reset_addr_latch();

    void fill_next_pixel();

    void fill_sprite_bitmaps();

    void fill_first_tiles();	

    void background_eval();

    void sprite_eval();

    void execute();

    uint8_t get_sprite_pixel();

    void display();
};

#endif
