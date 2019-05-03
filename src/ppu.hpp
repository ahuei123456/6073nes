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
    
    uint16_t vram_addr;

public:
    PPU(std::shared_ptr<Mem> memory);
    void set_reg(uint64_t index, uint8_t value);
    
    void set_oam();
    void execute();
};

#endif