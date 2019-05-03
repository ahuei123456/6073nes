#ifndef nes_hpp
#define nes_hpp

#include <cstdint>
#include <iostream>
#include <memory>
#include <chrono>

#include "rom.hpp"
#include "cpu.hpp"
#include "ppu.hpp"
#include "mem.hpp"

#define CYCLE_TIME      559
#define CYCLE_LITERAL   558.730073590338

class ROM;
class CPU;
class PPU;
class Mem;

class NES{
private:
    std::shared_ptr<ROM> rom;
    std::shared_ptr<CPU> cpu;
    std::shared_ptr<PPU> ppu;
    std::shared_ptr<Mem> memory;
    
    uint64_t cycles;
    int64_t cycles_until_ppu;
    
    std::chrono::high_resolution_clock::time_point prev;
    
public:
    NES(const char* filename);
    void run();
    void execute();
    
    void cpu_run();
};

#endif