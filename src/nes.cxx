#include "nes.hpp"

NES::NES(const char* filename) {
    cycles = 0;
    cycles_until_ppu = 3;
    
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        throw std::runtime_error(SDL_GetError());
    } 
    
    window = SDL_CreateWindow("6073NES", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, WIDTH, HEIGHT, SDL_WINDOW_SHOWN);
    if (window == NULL) {
        throw std::runtime_error(SDL_GetError());
    }
    
    rom = std::make_shared<ROM>(filename);
    memory = std::make_shared<Mem>(rom);
    
    cpu = std::make_shared<CPU>(memory);
    ppu = std::make_shared<PPU>(memory, window);
    
    memory->set_cpu(cpu);
    memory->set_ppu(ppu);
    
    prev_cycle = std::chrono::high_resolution_clock::now();
    prev_frame = prev_cycle;
    
    running = true;
}

void NES::run() {
    while (running) {
        auto current_cycle = std::chrono::high_resolution_clock::now();
        
        auto time_span = std::chrono::duration_cast<std::chrono::duration<double>>(current_cycle - prev_cycle);
        
        if (time_span > std::chrono::nanoseconds{CYCLE_TIME * passed}) {
            execute();
            prev_cycle = current_cycle;
        }
    }
}

void NES::cpu_run() {
    while (true) {
        cpu->execute();
    }
}

void NES::ppu_run() {
    while (true) {
        ppu->display();
    }
}

void NES::execute() {
    if (cycles_until_ppu <= 0) {
        ppu->execute();
        cycles_until_ppu += 3;
    } 
    
    passed = cpu->execute();
    if (passed == ERROR) {
        std::cout << "cycles: " << cycles << std::endl;
        running = false;
    } else {
        cycles += passed;
        cycles_until_ppu -= passed;
    }
    
}

NES::~NES() {
    SDL_DestroyWindow(window);
    window = NULL;
    SDL_Quit();
}