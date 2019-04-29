#include "mem.hpp"

Mem::Mem(std::shared_ptr<ROM> game) {
    if (game->get_mapper() == 0) {
        uint64_t size = game->get_prg_size();
        std::cout << "Memory mapper 0" << std::endl << "CHR size: " << size << " bytes" << std::endl;
        if (size == NROM_128) {
            for (int i = 0; i < NROM_128; i++) {
                uint8_t byte = game->get_prg(i);
                mem[NROM_START+i] = byte;
                mem[NROM_START+NROM_128+i] = byte;
            }
        } else if (size == NROM_256) {
            for (int i = 0; i < NROM_256; i++) {
                uint8_t byte = game->get_prg(i);
                mem[NROM_START+i] = byte;
            }
        }
    }
}

uint16_t Mem::reset_vector() {
    return mem[RESET_VECTOR] + (mem[RESET_VECTOR + 1] << 8);
}

uint16_t Mem::mem_read(uint64_t index) {
    return mem[index];
}

uint16_t Mem::mem_read2(uint64_t index) {
    return mem[index] + (mem[index + 1] << 8);
}

uint32_t Mem::mem_read3(uint64_t index) {
    return mem[index] + (mem[index + 1] << 8) + (mem[index + 2] << 16);
}

uint32_t Mem::mem_read4(uint64_t index) {
    return mem_read2(index) + (mem_read2(index + 2) << 16);
}

void Mem::mem_write(uint64_t index, uint8_t value) {
    if (index > 0x10000) {
        throw std::out_of_range("attempted to write to invalid memory address");
    }
    
    mem[index] = value;
}
