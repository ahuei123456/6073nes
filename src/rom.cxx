#include "rom.hpp"

ROM::ROM(const char* filename) {
    std::ifstream* rom = new std::ifstream;
    rom->open(filename, std::fstream::in | std::fstream::binary);
    
    if (!rom->is_open()) {
        throw std::invalid_argument("invalid filename");
    }
    
    this->read_header(rom);
    
    rom->close();
    delete rom;
}

void ROM::read_header(std::ifstream* rom) {
    for (int i = 0; i < INES_HEADER; i++) {
        header[i] = rom->get();
    }
    
    if (header[0] != 'N' || header[1] != 'E' || header[2] != 'S') {
        throw bad_rom();
    }
    
    prg_size = (header[4] + ((header[9] & 0x0f) << 8)) * PRG;
    chr_size = (header[5] + ((header[9] & 0xf0) << 4)) * CHR;
    
    has_trainer = (header[6] & 0xb) >> 2;
    
    if (has_trainer) {
        for (int i = 0; i < TRAINER; i++) {
            trainer[i] = rom->get();
        }
    }
    
    prg_rom.reserve(prg_size);
    for (int i = 0; i < prg_size; i++) {
        prg_rom.push_back(rom->get());
    }
    
    mapper = ((header[6] & 0x0f) >> 4) + (header[7] & 0x0f) + ((header[8] & 0xf) << 8);
    
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