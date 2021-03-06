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

#define NMI_VECTOR      0xFFFA
#define RESET_VECTOR    0xFFFC
#define VALID_ROM_INDEX(index) (index >= NROM_START && index < CPU_MEM_SIZE)
#define ACTUAL_ROM_ADDRESS(index) (index - NROM_START)

// input

#define NES_A       0
#define NES_B       1
#define NES_SELECT  2
#define NES_START   3
#define NES_UP      4
#define NES_DOWN    5
#define NES_LEFT    6
#define NES_RIGHT   7

#define JOYSTICK_1      0x4016
#define JOYSTICK_2      0x4017

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
class APU;

class Mem {
private:
    
    // cpu
    std::shared_ptr<CPU> cpu;
    std::array<uint8_t, RAM> ram;
    std::array<uint8_t, CPU_MEM_SIZE - NROM_START> prg_rom;
    
    bool flag_nmi = false;
    
    // input
    bool strobe = true;
    bool reading = false;
    uint8_t button = 0;
    bool pressed[8];
    
    // ppu
    std::shared_ptr<PPU> ppu;
    std::array<uint8_t, PATTERN_TABLE> left;
    std::array<uint8_t, PATTERN_TABLE> right;
    std::array<std::array<uint8_t, NAMETABLE>, 4> nametables;
    std::array<std::array<uint8_t, PALETTE>, 4> back_palettes;
    std::array<std::array<uint8_t, PALETTE>, 4> sprite_palettes;
    uint8_t univ_back_color;

    // ppu stuff accessible by cpu
    uint8_t ppu_latch;
    void oam_write(uint8_t value);
    
    std::shared_ptr<APU> apu;


public:
    
    // setup
    void set_cpu(std::shared_ptr<CPU> cpu);
    void set_ppu(std::shared_ptr<PPU> ppu);
    void set_apu(std::shared_ptr<APU> apu);

    // cpu only methods
    Mem(std::shared_ptr<ROM> game);
    uint16_t reset_vector();
    uint16_t nmi_vector();
    uint8_t mem_read(uint64_t index);
    uint16_t mem_read2(uint64_t index);
    void mem_write(uint64_t index, uint8_t value);
    bool read_nmi();
    
    // ppu only methods
    uint8_t ppu_read(uint64_t index);
    uint8_t ppu_write(uint64_t index, uint8_t value);

    // apu only methods
    void apu_reg_write(uint64_t index, uint8_t value);
    uint8_t apu_reg_read(uint64_t);
    uint8_t ppu_reg_read(uint64_t index);
    void ppu_reg_write(uint64_t index, uint8_t value);
    void set_nmi(bool nmi);
    uint64_t get_cpu_cycle();
    
    std::array<uint8_t, NAMETABLE> get_nametable(uint8_t index);
    std::array<uint8_t, PATTERN_TABLE> get_pattern_table(uint8_t index);
    std::array<std::array<uint8_t, PALETTE>, 4> get_back_palettes();
    uint8_t get_univ_back_color();
    
    // input
    void button_press(uint8_t button);
    void button_release(uint8_t button);
    
    // color
    uint32_t convert32(uint8_t value);
};

#endif
