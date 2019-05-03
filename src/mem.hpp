#ifndef mem_hpp
#define mem_hpp

#include <iostream>
#include <cstdint>
#include <array>
#include <memory>
#include "rom.hpp"
#include "cpu.hpp"

#define NROM_128 16384
#define NROM_256 32768
#define NROM_START 0x8000

#define CPU_MEM_SIZE 0x10000
#define PPU_MEM_SIZE 0x3FFF

#define RESET_VECTOR 0xFFFC

// ppu stuff

#define PPUCTRL     0x2000
#define PPUMASK     0x2001
#define PPUSTATUS   0x2002
#define OAMADDR     0x2003
#define OAMDATA     0x2004
#define PPUSCROLL   0x2005
#define PPUADDR     0x2006
#define PPUDATA     0x2007
#define OAMDMA      0x4014

#define VALID_PPU_INDEX(index) (index >= 0x2000 && index <= 0x3FFF)
#define ACTUAL_ADDRESS(index) ((index - 0x2000) % 8 + 0x2000)
#define PPU_REGISTER_WRITABLE(index) (!(index == 0x2002))
#define PPU_REGISTER_READABLE(index) (index == 0x2002 || index == 0x2004 || index == 0x2007)

class ROM;

class Mem {
private: 
    std::array<uint8_t, CPU_MEM_SIZE> cpu_mem;
    std::array<uint8_t, PPU_MEM_SIZE> ppu_mem;
    
    // ppu stuff accessible by cpu
    uint8_t ppu_reg_read(uint64_t index);
    void ppu_reg_write(uint64_t index, uint8_t value);
    uint8_t ppu_latch;
    void oam_write(uint8_t value);
    
public:

    // cpu only methods
    Mem(std::shared_ptr<ROM> game);
    uint16_t reset_vector();
    uint8_t mem_read(uint64_t index);
    uint16_t mem_read2(uint64_t index);

    void mem_write(uint64_t index, uint8_t value);
    
    // ppu only methods
};

#endif