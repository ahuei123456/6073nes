#ifndef mem_hpp
#define mem_hpp

#include <iostream>
#include <cstdint>
#include <array>
#include <memory>
#include "rom.hpp"
#include "cpu.hpp"
#include "ppu.hpp"
#include "apu.hpp"

#define NROM_128        16384
#define NROM_256        32768
#define NROM_START      0x8000

#define CPU_MEM_SIZE    0x10000
#define PPU_MEM_SIZE    0x3FFF

#define VALID_CPU_INDEX(index) (index < CPU_MEM_SIZE)

#define RAM             0x800
#define VALID_RAM_INDEX(index) (index >= 0 && index < PPU_START)
#define ACTUAL_RAM_ADDRESS(index) (index % RAM)

#define RESET_VECTOR    0xFFFC
#define VALID_ROM_INDEX(index) (index >= NROM_START && index < CPU_MEM_SIZE)
#define ACTUAL_ROM_ADDRESS(index) (index - NROM_START)

// ppu stuff

#define PATTERN_TABLE   0x1000
#define NAMETABLE       0x400
#define PALETTE         0x3

#define PPU_START       0x2000
#define PPUCTRL         0x2000
#define PPUMASK         0x2001
#define PPUSTATUS       0x2002
#define OAMADDR         0x2003
#define OAMDATA         0x2004
#define PPUSCROLL       0x2005
#define PPUADDR         0x2006
#define PPUDATA         0x2007
#define OAMDMA          0x4014

#define VALID_PPU_INDEX(index) (index >= 0x2000 && index <= 0x3FFF)
#define VALID_PPU_MEM_INDEX(index) (index >= 0 && index <= 0x3FFF)
#define VALID_APU_INDEX(index) ((index >= 0x4000 && index <= 0x4008) || (index >= 0x400A && index <= 0x400C) || (index >= 0x400E && index <= 0x4013) || (index == 0x4015) || (index == 0x4017))
#define ACTUAL_PPU_REGISTER(index) ((index - 0x2000) % 8 + 0x2000)
#define PPU_REGISTER_WRITABLE(index) (!(index == 0x2002))
#define PPU_REGISTER_READABLE(index) (index == 0x2002 || index == 0x2004 || index == 0x2007)
#define APU_REGISTER_READABLE(index) (index == 0x4015)

//apu stuff

#define APUSTATUS       0x4015
#define FRAME_COUNTER   0x4017

class ROM;
class CPU;
class PPU;

class Mem {
private:
    std::array<uint8_t, CPU_MEM_SIZE> cpu_mem;
    
    // cpu
    std::shared_ptr<CPU> cpu;
    std::array<uint8_t, RAM> ram;
    std::array<uint8_t, CPU_MEM_SIZE - NROM_START> prg_rom;
    
    // ppu
    std::shared_ptr<PPU> ppu;
    std::array<uint8_t, PATTERN_TABLE> left;
    std::array<uint8_t, PATTERN_TABLE> right;
    std::array<std::array<uint8_t, NAMETABLE>, 4> nametables;
    std::array<std::array<uint8_t, PALETTE>, 4> back_palettes;
    std::array<std::array<uint8_t, PALETTE>, 4> sprite_palettes;
    uint8_t univ_back_color;

    // ppu stuff accessible by cpu
    uint8_t ppu_reg_read(uint64_t index);
    void ppu_reg_write(uint64_t index, uint8_t value);
    uint8_t ppu_latch;
    void oam_write(uint8_t value);
    
    std::shared_ptr<APU> apu;


public:
    
    // setup
    void set_cpu(std::shared_ptr<CPU> cpu);
    void set_ppu(std::shared_ptr<PPU> ppu);

    // cpu only methods
    Mem(std::shared_ptr<ROM> game);
    uint16_t reset_vector();
    uint8_t mem_read(uint64_t index);
    uint16_t mem_read2(uint64_t index);
    void mem_write(uint64_t index, uint8_t value);
    
    // ppu only methods
    uint8_t ppu_read(uint64_t index);
    uint8_t ppu_write(uint64_t index, uint8_t value);

    // apu only methods
    void apu_reg_write(uint64_t index, uint8_t value);
    uint8_t apu_reg_read(uint64_t);
};

#endif
