#include "mem.hpp"

Mem::Mem(std::shared_ptr<ROM> game) {
    if (game->get_mapper() == 0) {
        uint64_t size = game->get_prg_size();
        std::cout << "Memory mapper 0" << std::endl << "CHR size: " << size << " bytes" << std::endl;
        if (size == NROM_128) {
            for (int i = 0; i < NROM_128; i++) {
                uint8_t byte = game->get_prg(i);
                cpu_mem[NROM_START+i] = byte;
                cpu_mem[NROM_START+NROM_128+i] = byte;
            }
        } else if (size == NROM_256) {
            for (int i = 0; i < NROM_256; i++) {
                uint8_t byte = game->get_prg(i);
                cpu_mem[NROM_START+i] = byte;
            }
        }
    }
}

uint16_t Mem::reset_vector() {
    return cpu_mem[RESET_VECTOR] + (cpu_mem[RESET_VECTOR + 1] << 8);
}

uint8_t Mem::mem_read(uint64_t index) {
    return cpu_mem[index];
}

uint16_t Mem::mem_read2(uint64_t index) {
    return cpu_mem[index] + (cpu_mem[index + 1] << 8);
}

uint32_t Mem::mem_read3(uint64_t index) {
    return cpu_mem[index] + (cpu_mem[index + 1] << 8) + (cpu_mem[index + 2] << 16);
}

uint32_t Mem::mem_read4(uint64_t index) {
    return mem_read2(index) + (mem_read2(index + 2) << 16);
}

void Mem::mem_write(uint64_t index, uint8_t value) {
    if (index > 0x10000) {
        throw std::out_of_range("attempted to write to invalid memory address");
    }
    
    if (VALID_PPU_INDEX(index)) {
        //ppu_mem_write(index, value);
    } else if (index == OAMDMA) {
        oam_write(value);
    }
    
    cpu_mem[index] = value;
}

// remember to actually update the PPU data

uint8_t Mem::ppu_reg_read(uint64_t index) {
    if (!VALID_PPU_INDEX(index)) {
        throw std::out_of_range("attempte to write to a non-ppu register!");
    }
    
    index = ACTUAL_ADDRESS(index);
    
    if (PPU_REGISTER_READABLE(index)) {
        uint8_t r_val = cpu_mem[index];
        if (index == PPUSTATUS) {
            cpu_mem[index] &= 0xef;
        } else if (index == PPUDATA) {
            // if VRAM address is under 0x3EFF (before palettes), return contents of read buffer
            // only updated when reading PPUDATA
            // https://wiki.nesdev.com/w/index.php/PPU_programmer_reference
        }
        
        ppu_latch = r_val;
    } else {
        return ppu_latch;
    }
}

// remember to actually write to the PPU

void Mem::ppu_reg_write(uint64_t index, uint8_t value) {
    if (!VALID_PPU_INDEX(index)) {
        throw std::out_of_range("attempte to read from a non-ppu register!");
    }
    
    index = ACTUAL_ADDRESS(index);
    
    ppu_latch = value;
    if (PPU_REGISTER_WRITABLE(index)) {
        cpu_mem[index] = value;
        
        if (index == OAMDATA) {
            cpu_mem[OAMADDR]++;
        } else if (index == PPUSCROLL) {
            //??
        } else if (index == PPUADDR) {
            //??
        } else if (index == PPUDATA) {
            // increment VRAM address after
            // but where is VRAM?
            // https://wiki.nesdev.com/w/index.php/PPU_programmer_reference
        }
    }
}

void Mem::oam_write(uint8_t value) {
    
}