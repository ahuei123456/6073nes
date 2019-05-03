#include "nes.hpp"

NES::NES(const char* filename) {
    cycles = 0;
    cycles_until_ppu = 3;
    
    rom = std::make_shared<ROM>(filename);
    memory = std::make_shared<Mem>(rom);
    
    cpu = std::make_shared<CPU>(memory);
    ppu = std::make_shared<PPU>(memory);
    
    memory->set_cpu(cpu);
    memory->set_ppu(ppu);
    
    prev = std::chrono::high_resolution_clock::now();
}

void NES::run() {
    while (true) {
        auto now = std::chrono::high_resolution_clock::now();
        
        auto time_span = std::chrono::duration_cast<std::chrono::duration<double>>(now - prev);
        
        if (time_span > std::chrono::nanoseconds{CYCLE_TIME}) {
            execute();
            prev = now;
        }
    }
}

void NES::cpu_run() {
    while(true) {
        cpu->execute();
    }
}

void NES::execute() {
    if (cycles_until_ppu <= 0) {
        ppu->execute();
        cycles_until_ppu += 3;
    } else {
        uint16_t passed = cpu->execute();
        cycles += passed;
        cycles_until_ppu -= passed;
    }
}