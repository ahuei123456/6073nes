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

void PPU::execute() {
         
    uint16_t nametable_index = (vram_addr >> 10) & 0x4;	
    uint16_t nametable_addr = (nametable_index << 10) + 4096;
    uint8_t coarse_x = vram_addr % 32;
    uint8_t coarse_y = (vram_addr >> 5) % 32;
    if (cycle_modulo_8 == 1) 
        uint16_t byte_addr = nametable_addr + coarse_x + coarse_y * 32;
	nametable_byte = memory->ppu_read(byte_addr);
    }

    else if (cycle_modulo_8 == 3) {
	uint8_t att_y = coarse_y / 4;
	uint8_t att_x = coarse_x / 4;
	attribute_byte = memory->ppu_read(nametable_addr + 960 + att_x + 8 * att_y);

    }

    else if (cycle_modulo_8 == 5) {
        uint8_t offset = (vram_addr >> 12) % 8;
	uint16_t tile_address = (nametable_byte << 4) + offset;
	tile_low = memory->ppu_read(tile_address);	
    }

    else if (cycle_modulo_8 == 7)
    {
	uint8_t fine_y = (vram_addr >> 12) % 8;
        uint16_t tile_address = (nametable_byte << 4) + 8 + fine_y;
        tile_high = memory->ppu_read(tile_address);	
	
	pixel_array[fine_y][fine_x]
	
	
	fine_x = (fine_x + 1) % 256;
	if (fine_x % 8 == 0) {
	    coarse_x++;
            if (fine_x == 0) {
		current_scanline = (current_scanline + 1) % 262;
		fine_y = (fine_y + 1) % 8;
		if (current_scanline % 8 == 0) {
		    coarse_y += 1;
	        }
	    }
	    vram_addr = ((uint16_t) fine_y << 12) + (nametable_index << 10) + ((uint16_t) coarse_y << 5) + coarse_x;
	}
    }
 
    cycle_modulo_8 = (cycle_modulo_8 + 1) % 8;
}

