#include "cpu.hpp"

CPU::CPU(std::shared_ptr<Mem> memory) {
    this->memory = memory;
    
    reg_pc = memory->reset_vector();
}

void CPU::execute() {
    uint16_t opcode = memory->mem_read(reg_pc);
    
    switch (opcode) {
        case ADC_I: {
            // do something
            break;
        }
        // add all the other opcodes
    }
}