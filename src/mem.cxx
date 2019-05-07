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
    }
}

// remember to actually update the PPU data

uint8_t Mem::ppu_reg_read(uint64_t index) {
    if (!VALID_PPU_INDEX(index)) {
        throw std::out_of_range("attempte to write to a non-ppu register!");
    }
    
    index = ACTUAL_PPU_REGISTER(index);
    
    if (PPU_REGISTER_READABLE(index)) {
        uint8_t r_val = cpu_mem[index];
        if (index == PPUSTATUS) {
            cpu_mem[index] &= 0xef;
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
        
        if (index == OAMDATA) {
            cpu_mem[OAMADDR]++;
        } else if (index == PPUSCROLL) {
	    ppu->set_scroll_coord(value);
        } else if (index == PPUADDR) {
	    ppu->set_vram_addr(value);

        } else if (index == PPUDATA) {
	    //Writes data to appropriate VRAM address
            ppu_mem[ppu->get_vram_addr()] = value;
            
	    //This is the 2nd bit of PPUCTRL, which determines how much VRAM is incremented
	    uint8_t incBit = (cpu_mem[PPUCTRL] >> 2) & (0x1);
	    ppu->inc_vram_addr(incBit);
	    // increment VRAM address after
            // but where is VRAM?
            // https://wiki.nesdev.com/w/index.php/PPU_programmer_reference
        }
    }
}

uint8_t Mem::ppu_read(uint64_t index) {
    return ppu_mem[index];
}

void Mem::oam_write(uint8_t value) {
    ppu->set_oam(value);    
}
