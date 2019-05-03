#include <iostream>
#include <SDL2/SDL.h>
#include <memory>
#include <iomanip>

#include "rom.hpp"
#include "mem.hpp"

int main(int argc, const char * argv[]) {
    const char* filename = argv[1];
    auto game = std::make_shared<ROM>(filename);
    auto memory = std::make_shared<Mem>(game);
    auto start = memory->reset_vector();
    std::cout << "start: " << start << std::endl;
    //auto opcode = memory->mem_read(start);
    //std::cout << "opcode: " << std::hex << opcode << std::endl;
    
    auto cpu = std::make_shared<CPU>(memory);
    cpu->execute();
    cpu->execute();
    return 0;
}