#include <iostream>
#include <SDL2/SDL.h>
#include <memory>
#include <iomanip>

#include "nes.hpp"

int main(int argc, const char * argv[]) {
    const char* filename = argv[1];
    auto nes = std::make_shared<NES>(filename);
    nes->run();
    return 0;
}