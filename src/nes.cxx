#include "nes.hpp"

NES::NES(const char* filename) {
    cycles = 0;
    cycles_until_ppu = 3;
    
    rom = std::make_shared<ROM>(filename);
    memory = std::make_shared<Mem>(rom);
    
    cpu = std::make_shared<CPU>(memory);
    ppu = std::make_shared<PPU>(memory);
    
    memory->set_cpu(cpu);
    memory->set_ppu(ppu);
    
    prev = std::chrono::high_resolution_clock::now();
    
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        throw std::runtime_error(SDL_GetError());
    } 
    
    window = std::shared_ptr<SDL_Window>(SDL_CreateWindow("6073NES", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, WIDTH, HEIGHT, SDL_WINDOW_SHOWN), SDL_DestroyWindow);
    if (window == NULL) {
        throw std::runtime_error(SDL_GetError());
    }
    
    screen = std::shared_ptr<SDL_Surface>(SDL_GetWindowSurface(window.get()), SDL_FreeSurface);
    
    SDL_FillRect(screen.get(), NULL, SDL_MapRGB(screen->format, 0xFF, 0xFF, 0xFF));
    SDL_UpdateWindowSurface(window.get());
}

void NES::run() {
    while (true) {
        auto now = std::chrono::high_resolution_clock::now();
        
        auto time_span = std::chrono::duration_cast<std::chrono::duration<double>>(now - prev);
        
        if (time_span > std::chrono::nanoseconds{CYCLE_TIME}) {
            execute();
            prev = now;
        }
    }
}

void NES::cpu_run() {
    while(true) {
        cpu->execute();
    }
}

void NES::execute() {
    if (cycles_until_ppu <= 0) {
        ppu->execute();
        cycles_until_ppu += 3;
    } else {
        uint16_t passed = cpu->execute();
        if (passed == ERROR) {
            std::cout << "cycles: " << cycles << std::endl;
            exit(0);
        } else {
            cycles += passed;
            cycles_until_ppu -= passed;
        }
    }
}