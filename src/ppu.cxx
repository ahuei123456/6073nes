#include "ppu.hpp"

uint8_t Sprite::palette() {
    return attributes & 0x3;
}

uint8_t Sprite::priority() {
    return (attributes & 0x20) >> 5;
}

uint8_t Sprite::horizontal_flip() {
    return (attributes & 0x40) >> 6;
}

uint8_t Sprite::vertical_flip() {
    return (attributes & 0x80) >> 7;
}

PPU::PPU(std::shared_ptr<Mem> memory) {
    this->memory = memory;
}

void PPU::set_reg(uint64_t index, uint8_t value) {
    if (index >= REGS) {
        throw std::out_of_range("invalid ppu register index");
    }
    
    regs[index] = value;
}