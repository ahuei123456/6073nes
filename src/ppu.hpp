#ifndef ppu_hpp
#define ppu_hpp

#include <cstdint>
#include <iostream>
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

#define WIDTH           256
#define HEIGHT          240
#define ADDR(X, Y) (Y * WIDTH + X)

#define PALETTE_32 {0x656565FF, 0x002D69FF, 0x131F7FFF, 0x3C137CFF, 0x600B62FF, 0x730A37FF, 0x710F07FF, 0x5A1A00FF, 0x342800FF, 0x0B3400FF, 0x003C00FF, 0x003D10FF, 0x003840FF, 0x000000FF, 0x000000FF, 0x000000FF, \
                    0xAEAEAEFF, 0x0F63B3FF, 0x4051D0FF, 0x7841CCFF, 0xA736A9FF, 0xC03470FF, 0xBD3C30FF, 0x9F4A00FF, 0x6D5C00FF, 0x366D00FF, 0x077704FF, 0x00793DFF, 0x00727DFF, 0x000000FF, 0x000000FF, 0x000000FF, \
                    0xFEFEFFFF, 0x5DB3FFFF, 0x8FA1FFFF, 0xC890FFFF, 0xF785FAFF, 0xFF83C0FF, 0xFF8B7FFF, 0xEF9A49FF, 0xBDAC2CFF, 0x85BC2FFF, 0x55C753FF, 0x3CC98CFF, 0x3EC2CDFF, 0x4E4E4EFF, 0x000000FF, 0x000000FF, \
                    0xFEFEFFFF, 0xBCDFFFFF, 0xD1D8FFFF, 0xE8D1FFFF, 0xFBCDFDFF, 0xFFCCE5FF, 0xFFCFCAFF, 0xF8D5B4FF, 0xE4DCA8FF, 0xCCE3A9FF, 0xB9E8B8FF, 0xAEE8D0FF, 0xAFE5EAFF, 0xB6B6B6FF, 0x000000FF, 0x000000FF}

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
    uint8_t byte(uint8_t index);
};

class PPU {
private:
    // Pointer to overall memory
    std::shared_ptr<Mem> memory;
    // PPU registers
    std::array<uint8_t, REGS> regs;
    // OAM memory
    std::array<Sprite, SPRITES> oam;
    std::array<Sprite, SPRITES_SEC> oam_sec;
    
    uint8_t read_buffer;
    uint8_t primary_oam_byte;
    uint8_t n = 0;
    uint8_t m = 0;
    uint8_t oam_sec_index = 0;
    bool oam_sec_full = false;
    bool in_range = false;

    // PPU memory used for rendering specified in documentation
    uint16_t vram_addr;
    uint16_t temp_vram_addr;
    std::array<uint8_t, SPRITES_SEC> sprite_bitmap_low;
    std::array<uint8_t, SPRITES_SEC> sprite_bitmap_high;
    std::array<uint8_t, SPRITES_SEC> sprite_attributes;
    std::array<uint8_t, SPRITES_SEC> sprite_x;
    uint8_t attribute_byte_1, attribute_byte_2;
    uint16_t background_bitmap_1, background_bitmap_2;
    
    uint8_t fine_x;
    uint8_t temp_fine_x;

    // Determines whether x or y coordinate is set next. If false, x-coordinate. If true, y-coordinate.
    bool addr_latch = false;
    uint16_t bitmap_latch;
    uint8_t sprite_x_latch;
    uint8_t sprite_attribute_latch;
    uint8_t sprite_tile_latch;
    uint8_t sprite_y_latch;


    //Variables used to keep track of where we are in cycles/scanlines.
    uint16_t current_scanline = 261;
    uint8_t cycle_mod_8 = 0;
    uint8_t cycle_mod_341 = 0;

    uint8_t nametable_byte;
    bool sprite_foreground;
    uint8_t attribute_byte;

    std::array<std::array<uint8_t, 256>, 240> pixel_array;
    std::array<uint32_t, 64> color_map = PALETTE_32;
    
    // SDL
    SDL_Window* window;
    SDL_Renderer* renderer;
    SDL_Texture* screen;
    
    uint32_t* get_pixel_array();
    
    uint64_t total_cycles = 0;

public:
    PPU(std::shared_ptr<Mem> memory, SDL_Window* window);
    ~PPU();
    void set_oam(uint8_t byte);
    void set_vram_addr(uint8_t value);
    uint16_t get_vram_addr();
    void set_reg(uint64_t index, uint8_t value);
    uint8_t read_reg(uint64_t index);

    void inc_vram_addr(uint8_t type);
    void set_scroll_coord(uint8_t value);
    void update_buffer();
    uint8_t get_buffer();
    void reset_addr_latch();

    void decrement_sprite_counter();	

    void fill_next_pixel();
    void fill_sprite_bitmaps();
    void fill_first_tiles();	
    void background_eval();

    uint8_t sprite_byte(Sprite sprite, uint8_t index);

    void sprite_eval();
    void execute();
    uint8_t get_sprite_pixel();
    void display();
};

#endif
