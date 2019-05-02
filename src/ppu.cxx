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

void PPU::set_oam(uint8_t byte) {
//Sets all of OAM to the data on the corresponding input page.
    uint8_t word_addr = byte << 8;
    for (int i = 0; i < 0xFF; i+= 4)
    {//Each sprite has 4 bytes of data. We fill the 64 sprites in.
	 int sprite_index = i / 4;
	 oam[sprite_index]::Y = memory::mem_read(word_addr);
	 oam[sprite_index]::index = memory::mem_read(word_addr + 1);
	 oam[sprite_index]::attributes = memory::mem_read(word_addr + 2);
	 oam[sprite_index]::X = memory::mem_read(word_addr + 3);
    }
}

void PPU::set_vram_addr(uint8_t value) {
//Setting the new vram address shifts left the lowest 8 bits and then adds a byte value.
    if (!addr_latch)
    {
	vram_addr = ((uint16_t) value << 8) + (vram_addr % 0xFF); 
    }

    else
    {
	vram_addr = ((vram_addr >> 8) << 8) + value;
    }

    addr_latch = !addr_latch;
}

uint16_t PPU::get_vram_addr() {
    return vram_addr;
}

void PPU::inc_vram_addr(uint8_t type) {
    if (type == 0)
    {
	vram_addr += 1;
    }

    else
    {
	vram_addr += 32;
    }
}

void PPU::set_scroll_coord(uint8_t value) {
    if (!addr_latch)
    {
	scroll_x = value;
    }

    else
    {
	scroll_y = value;
    }

    addr_latch = !addr_latch;
}

uint8_t PPU::get_buffer() {
    return read_buffer;
}

void PPU::update_buffer() {
    read_buffer = ppu_read(vram_addr);
}

void PPU::reset_addr_latch() {
    addr_latch = false;
}

