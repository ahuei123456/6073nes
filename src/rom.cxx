#include "rom.hpp"

ROM::ROM(const char* filename) {
    std::ifstream* rom = new std::ifstream;
    rom->open(filename, std::fstream::in | std::fstream::binary);
    
    if (!rom->is_open()) {
        std::cout << "failed to open rom!" << std::endl;
        exit(0);
    }
    
    this->read_header(rom);
}

void ROM::read_header(std::ifstream* rom) {
    for (int i = 0; i < INES_HEADER; i++) {
        header[i] = rom->get();
    }
    
    if (header[0] != 'N' || header[1] != 'E' || header[2] != 'S') {
        throw bad_rom();
    }
    
    std::cout << unsigned(header[4]) << " " << unsigned(header[5]) << std::endl;
    std::cout << unsigned(header[9]) << std::endl;
    
    prg_size = (header[4] + ((header[9] & 0x0f) << 8)) * PRG;
    chr_size = (header[5] + ((header[9] & 0xf0) << 4)) * CHR;
    
    std::cout << prg_size << " " << chr_size << std::endl;
    
    trainer = (header[6] & 0xb) >> 2;
    
    if (trainer) {
        std::cout << "trainer present" << std::endl;
        for (int i = 0; i < 512; i++) {
            rom->get();
        }
    }
    
    prg_rom.reserve(prg_size);
    for (int i = 0; i < prg_size; i++) {
        prg_rom.push_back(rom->get());
    }
    
    mapper = ((header[6] & 0x0f) >> 4) + (header[7] & 0x0f) + ((header[8] & 0xf) << 8);
    std::cout << "mapper: " << mapper << std::endl;
    
    chr_rom.reserve(chr_size);
    for (int i = 0; i < chr_size; i++) {
        chr_rom.push_back(rom->get());
    }
}

uint8_t ROM::get_prg(uint64_t index) {
    return prg_rom.at(index);
}    

uint8_t ROM::get_chr(uint64_t index) {
    return chr_rom.at(index);
}

uint32_t ROM::get_prg_size() {
    return prg_size;
}

uint32_t ROM::get_chr_size() {
    return chr_size;
}

uint32_t ROM::get_mapper() {
    return mapper;
}