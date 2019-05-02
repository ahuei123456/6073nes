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
    std::shared_ptr<Mem> memory;

    std::array<uint8_t, REGS> regs;
    std::array<Sprite, SPRITES> oam;
    std::array<Sprite, SPRITES_SEC> oam_sec;
    uint8_t read_buffer;

    uint16_t vram_addr;
    //Positions for upper left corner of screen
    uint8_t scroll_x;
    uint8_t scroll_y;
    //Determines whether x or y coordinate is set next. If false, x-coordinate. If true, y-coordinate.
    bool addr_latch = false;

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
};

#endif
