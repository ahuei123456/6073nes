#ifndef rom_hpp
#define rom_hpp

#include <cstdint>
#include <vector>
#include <array>

#include <fstream>
#include <iostream>
#include <vector>
#include <iterator>

#include <exception>

#define INES_HEADER 16
#define PRG 16384
#define CHR 8192

class ROM {
private:
    std::vector<uint8_t> prg_rom;
    std::vector<uint8_t> chr_rom;
    std::array<uint8_t, INES_HEADER> header;
    uint32_t prg_size;
    uint32_t chr_size;
    uint32_t mapper;
    
    bool trainer;
    void read_header(std::ifstream* rom);
    
public:
    ROM(const char* filename);
    uint8_t get_prg(uint64_t index);
    uint8_t get_chr(uint64_t index);
    uint32_t get_prg_size();
    uint32_t get_chr_size();
    uint32_t get_mapper();
};

struct bad_rom : public std::exception {
    const char* what () const throw () {
        return "bad header";
    }
};

#endif