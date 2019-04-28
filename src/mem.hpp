#ifndef mem_hpp
#define mem_hpp

#include <iostream>
#include <cstdint>
#include <array>
#include <memory>
#include <exception>
#include "rom.hpp"

#define NROM_128 16384
#define NROM_256 32768
#define NROM_START 0x8000

#define RESET_VECTOR 0xFFFC

class ROM;

class Mem {
private: 
    std::array<uint8_t, 0x10000> mem;
    
public:
    Mem(std::shared_ptr<ROM> game);
    uint16_t reset_vector();
    uint16_t mem_read(uint64_t index);
    uint16_t mem_read2(uint64_t index);
    uint32_t mem_read3(uint64_t index);
    uint32_t mem_read4(uint64_t index);
    
    void mem_write(uint64_t index, uint8_t value);
};

#endif