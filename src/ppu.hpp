#ifndef ppu_hpp
#define ppu_hpp

#include <cstdint>
#include <iostream>
#include <array>
#include <exception>
#include <memory>
#include <queue>
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

#define NAMETABLE_ADDR() (get_nametable_index() * NAMETABLE + 0x2000)

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
    
    void ff();
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
    
    uint8_t read_buffer;
    uint8_t primary_oam_byte;
    uint8_t oam_sec_index = 0;
    bool oam_sec_full = false;
    bool in_range = false;

    // PPU memory used for rendering specified in documentation
    
    // background
    uint16_t vram_addr;
    uint16_t temp_vram_addr;
    uint8_t fine_x;
    uint8_t write_toggle;
    uint16_t background_bitmap_1, background_bitmap_2;
    uint8_t palette_attribute_1, palette_attribute_2;
    
    // sprites
    std::array<Sprite, SPRITES> oam;
    std::array<Sprite, SPRITES_SEC> oam_sec;
    std::array<uint8_t, SPRITES_SEC> sprite_bitmap_low;
    std::array<uint8_t, SPRITES_SEC> sprite_bitmap_high;
    std::array<uint8_t, SPRITES_SEC> sprite_attributes;
    std::array<uint8_t, SPRITES_SEC> sprite_x;
    void decrement_sprite_counter();
    uint8_t get_sprite_pixel();

    // Determines whether x or y coordinate is set next. If false, x-coordinate. If true, y-coordinate.
    bool addr_latch = false;
    uint16_t bitmap_latch;
    uint8_t sprite_x_latch;
    uint8_t sprite_attribute_latch;
    uint8_t sprite_tile_latch;
    uint8_t sprite_y_latch;

    // timing
    uint64_t cycles = 0;
    uint16_t current_scanline = 261;
    
    // latches
    uint8_t nametable_byte;
    bool sprite_foreground;
    uint8_t attribute_byte;
    
    void inc_cycle();
    void inc_scanline();

    std::array<std::array<uint8_t, 256>, 240> pixel_array;
    std::array<uint32_t, 64> color_map = PALETTE_32;
        
    // PPUCTRL info
    uint16_t get_base_nametable_addr();
    uint8_t get_vram_increment();
    uint16_t get_sprite_pattern_table_addr();
    uint16_t get_background_pattern_table_addr();
    uint8_t get_sprite_height();
    bool get_ppu_select();
    bool get_vblank_nmi_flag();
    
    // PPUMASK info
    bool get_greyscale();
    bool get_background_left_flag();
    bool get_sprite_left_flag();
    bool get_background_flag();
    bool get_sprite_flag();
    bool get_red_flag();
    bool get_green_flag();
    bool get_blue_flag();
    
    // PPUSTATUS
    void set_overflow_flag(bool value);
    void set_sprite_0_hit_flag(bool value);
    void set_vblank_flag(bool value);
    
    // scrolling
    void inc_coarse_x();
    void inc_fine_y();
    uint8_t get_coarse_x();
    uint8_t get_coarse_y();
    uint8_t get_nametable_index();
    uint8_t get_fine_y();
    
    // rendering
    std::queue<uint8_t> pipeline;
    uint16_t bkg_addr;
    uint8_t low_pattern;
    uint8_t high_pattern;
    Sprite sprite_buffer;
    uint8_t n = 0;
    uint8_t m = 0;
    uint8_t sprites_found = 0;
    
    uint16_t get_tile_address();
    uint16_t get_attribute_address();
    
    uint8_t get_sprite_bitmap_low(Sprite sprite);
    uint8_t get_sprite_bitmap_high(Sprite sprite);
    
    void scanl_bkg();
    void scanl_spr();
    void render_pixel();
    
    void inc_fine_x();
    void fill_next_pixel();
    void fill_sprite_bitmaps();
    void scan();
    void scan_261();	
    void background_eval();
    void sprite_eval();
    
    // SDL
    SDL_Window* window;
    SDL_Renderer* renderer;
    SDL_Texture* screen;
    
    uint32_t* get_pixel_array();
    uint32_t convert32(uint8_t value);
    
public:
    PPU(std::shared_ptr<Mem> memory, SDL_Window* window);
    ~PPU();
    void set_oam(uint8_t byte);
    void set_vram_addr(uint8_t value);
    uint16_t get_vram_addr();
    void ext_reg_write(uint64_t index, uint8_t value);
    uint8_t ext_reg_read(uint64_t index);

    void execute();
    void display();
};

#endif
