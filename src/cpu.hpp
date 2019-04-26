#ifndef cpu_hpp
#define cpu_hpp

#include <cstdint>
#include <iostream>
#include <vector>
#include <memory>

#include "mem.hpp"

class Mem;

class CPU {
private:
    std::shared_ptr<Mem> memory;
    
    // register info
    // https://wiki.nesdev.com/w/index.php/CPU_registers
    
    // accumulator
    uint8_t reg_a;
    
    // indexes
    uint8_t reg_x;
    uint8_t reg_y;
    
    // pc
    uint16_t reg_pc;
    
    // sp
    uint8_t reg_s;
    
    // status register
    uint8_t reg_p;
    
public:
    CPU(std::shared_ptr<Mem> memory);
    void execute();
};

#endif