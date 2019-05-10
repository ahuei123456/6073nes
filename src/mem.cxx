#include "mem.hpp"

Mem::Mem(std::shared_ptr<ROM> game) {
    if (game->get_mapper() == 0) {
        uint64_t size = game->get_prg_size();
        //std::cout << "Memory mapper 0" << std::endl << "CHR size: " << size << " bytes" << std::endl;
        if (size == NROM_128) {
            for (int i = 0; i < NROM_128; i++) {
                uint8_t byte = game->get_prg(i);
                prg_rom[i] = byte;
                prg_rom[NROM_128+i] = byte;
            }
        } else if (size == NROM_256) {
            for (int i = 0; i < NROM_256; i++) {
                uint8_t byte = game->get_prg(i);
                prg_rom[i] = byte;
            }
        }
    } 
}

void Mem::set_cpu(std::shared_ptr<CPU> cpu) {
    this->cpu = cpu;
}

void Mem::set_ppu(std::shared_ptr<PPU> ppu) {
    this->ppu = ppu;
}

void Mem::set_apu(std::shared_ptr<APU> apu) {
    this->apu = apu;
}

uint16_t Mem::reset_vector() {
    return mem_read2(RESET_VECTOR);
}

uint8_t Mem::mem_read(uint64_t index) {
    if (!VALID_CPU_INDEX(index)) {
        throw std::out_of_range("attempted to read from an invalid memory address");
    }
    
    if (VALID_RAM_INDEX(index)) {
        return ram[ACTUAL_RAM_ADDRESS(index)];
    } else if (VALID_PPU_INDEX(index)) {
        return ppu_reg_read(index);
    } else if (VALID_ROM_INDEX(index)) {
        return prg_rom[ACTUAL_ROM_ADDRESS(index)];
    } else if (VALID_APU_INDEX(index)) {
	return apu_reg_read(index);
    } else {
        // placeholder
        return 0;
    }
}

uint16_t Mem::mem_read2(uint64_t index) {
    return mem_read(index) + (mem_read(index + 1) << 8);
}

void Mem::mem_write(uint64_t index, uint8_t value) {
    if (index > 0x10000) {
        throw std::out_of_range("attempted to write to invalid memory address");
    }
    
    if (VALID_RAM_INDEX(index)) {
        ram[ACTUAL_RAM_ADDRESS(index)] = value;
    } else if (VALID_PPU_INDEX(index)) {
        ppu_reg_write(index, value);
    } else if (index == OAMDMA) {
        oam_write(value);
    } else if (VALID_APU_INDEX(index)) {
	apu_reg_write(index, value);
    }
}

// remember to actually update the PPU data

uint8_t Mem::ppu_reg_read(uint64_t index) {
    if (!VALID_PPU_INDEX(index)) {
        throw std::out_of_range("attempte to write to a non-ppu register!");
    }
    
    index = ACTUAL_PPU_REGISTER(index);
    
    if (PPU_REGISTER_READABLE(index)) {
        uint8_t r_val = ppu->read_reg(index % 8);
        if (index == PPUSTATUS) {
            ppu->set_reg(index % 8, r_val &= 0xEF);
            ppu->reset_addr_latch();
	} else if (index == PPUDATA) {
	    ppu->update_buffer();
            if (ppu->get_vram_addr() < 0x3EFF) {
		r_val = ppu->get_buffer();
	    }
		// if VRAM address is under 0x3EFF (before palettes), return contents of read buffer
            // only updated when reading PPUDATA
            // https://wiki.nesdev.com/w/index.php/PPU_programmer_reference
        }
        
        ppu_latch = r_val;
        return ppu_latch;
    } else {
        return ppu_latch;
    }
}

// remember to actually write to the PPU

void Mem::ppu_reg_write(uint64_t index, uint8_t value) {
    if (!VALID_PPU_INDEX(index)) {
        throw std::out_of_range("attempte to read from a non-ppu register!");
    }
    
    index = ACTUAL_PPU_REGISTER(index);
    
    ppu_latch = value;
    if (PPU_REGISTER_WRITABLE(index)) {
        cpu_mem[index] = value;
    	ppu->set_reg(index % 8, value);

        if (index == OAMDATA) {
            cpu_mem[OAMADDR]++;
        } else if (index == PPUSCROLL) {
	    ppu->set_scroll_coord(value);
        } else if (index == PPUADDR) {
	    ppu->set_vram_addr(value);

        } else if (index == PPUDATA) {
	    //Writes data to appropriate VRAM address
            ppu_write(ppu->get_vram_addr(), value);
            
	    //This is the 2nd bit of PPUCTRL, which determines how much VRAM is incremented
	    uint8_t incBit = (cpu_mem[PPUCTRL] >> 2) & (0x1);
	    ppu->inc_vram_addr(incBit);
	    // increment VRAM address after
            // but where is VRAM?
            // https://wiki.nesdev.com/w/index.php/PPU_programmer_reference
        }
    }
}

uint8_t Mem::ppu_read(uint64_t index)
{
    if (!VALID_PPU_MEM_INDEX(index)) {
	throw std::out_of_range("attempted to read from an invalid ppu memory address!");
    }

    if (index >= 0x3000 && index <= 0x3EFF) {
    //Addresses in this range are mirrors of the nametable addresses.
	index -= 0x1000;
    }

    else if (index >= 0x3F20 && index <= 0x3FFF) {
    //Addresses in this range are mirrors of the palette RAM addresses.
   	index = 0x3F00 + (index % 0x20);
    }

    //Now we find the corresponding area of memory the address belongs to and calculate the index to return.

    if (index >= 0 && index < 0x1000) {
	return left[index];
    }

    else if (index < 0x2000) {
	return right[index % 0x1000];
    }

    else if (index < 0x2400) {
	return nametables[0][index % 0x400]; 
    }

    else if (index < 0x2800) {
	return nametables[1][index % 0x400];
    }

    else if (index < 0x2C00) {
	return nametables[2][index % 0x400];
    }

    else if (index < 0x3000) {
	return nametables[3][index % 0x400];
    }

    else if (index >= 0x3F00 && index < 0x3F20) {
	uint8_t palette_num = (index % 0xF) / 4;
	uint8_t color_num = (index % 4);
	if (color_num == 0) {
	     return univ_back_color;
        }

	if (index < 0x3F10) {
	     return back_palettes[palette_num][color_num - 1];
	}

	else {
	     return sprite_palettes[palette_num][color_num - 1];
	}	
    }

}

uint8_t Mem::ppu_write(uint64_t index, uint8_t value) {
    if (!VALID_PPU_MEM_INDEX(index)) {
	throw std::out_of_range("Tried to write to invalid ppu memory address!");
    }

    if (index >= 0x3000 && index <= 0x3FFF) {
	index -= 0x1000;
    }

    else if (index >= 0x3F20 && index <= 0x3FFF) {
	index = 0x3F00 + (index % 0x20);	
    }

    if (index < 0x1000) {
	left[index] = value;
    }

    else if (index < 0x2000) {
	right[index % 0x1000] = value;
    }

    else if (index < 0x2400) {
	nametables[0][index % 0x400] = value;
    }

    else if (index < 0x2800) {
	nametables[1][index % 0x400] = value;
    }

    else if (index < 0x2C00) {
	nametables[2][index % 0x400] = value;
    }

    else if (index < 0x3000) {
	nametables[3][index % 0x400] = value;
    }

    else if (index >= 0x3F00 && index < 0x3F20) {
	uint8_t palette_num = (index % 0x10) / 4;
        uint8_t color_num = (index % 4);
	if (color_num == 0) {
	     univ_back_color = value;	
        }	

	else {
	     if (index < 0x3F10) {
		  back_palettes[palette_num][color_num - 1] = value;
	     }

	     else {
		  sprite_palettes[palette_num][color_num - 1] = value;
	     }
        }
    }
}

uint8_t Mem::apu_reg_read(uint64_t index) {
    return apu->reg_read(index);
}

void Mem::apu_reg_write(uint64_t index, uint8_t value) {
    apu->reg_write(index, value);
}

void Mem::oam_write(uint8_t value) {
    ppu->set_oam(value);    
}
