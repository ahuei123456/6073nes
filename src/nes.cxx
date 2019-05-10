#include "nes.hpp"

NES::NES(const char* filename) {
    cycles = 0;
    cycles_until_ppu = 3;
    event = new SDL_Event;
    
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
    //if (run_t < 1) exit(0);
    poll_input();
    
    passed = cpu->execute();
    if (false) {
        std::cout << cpu->get_inst();
        std::cout << ppu->debug();
        std::cout << std::endl;
    }
    
    if (passed == ERROR) {
        std::cout << "cycles: " << cycles << std::endl;
        running = false;
    } else {
        cycles += passed;
        for (int i = 0; i < passed * 3; i++) {
            ppu->execute();
        }
    }
    
    
    
    run_t--;
}

void NES::poll_input() {
    SDL_PollEvent(event);
    
    if (event->type == SDL_KEYDOWN) {
        uint64_t kdown = event->key.keysym.sym;
        switch (kdown) {
             case SDLK_UP: {
                 if (!kstate[NES_UP]) {
                     std::cout << "press up" << std::endl;
                     memory->button_press(NES_UP);
                     kstate[NES_UP] = true;
                 }
                 break;
             }
             case SDLK_DOWN: {
                 if (!kstate[NES_DOWN]) {
                     std::cout << "press down" << std::endl;
                     memory->button_press(NES_DOWN);
                     kstate[NES_DOWN] = true;
                 }
                 break;
             }
             case SDLK_RIGHT: {
                 if (!kstate[NES_RIGHT]) {
                     std::cout << "press right" << std::endl;
                     memory->button_press(NES_RIGHT);
                     kstate[NES_RIGHT] = true;
                 }
                 break;
             }
             case SDLK_LEFT: {
                 if (!kstate[NES_LEFT]) {
                     std::cout << "press left" << std::endl;
                     memory->button_press(NES_LEFT);
                     kstate[NES_LEFT] = true;
                 }
                 break;
             }
             case SDLK_z: {
                 if (!kstate[NES_A]) {
                     std::cout << "press A" << std::endl;
                     memory->button_press(NES_A);
                     kstate[NES_A] = true;
                 }
                 break;
             }
             case SDLK_x: {
                 if (!kstate[NES_B]) {
                     std::cout << "press B" << std::endl;
                     memory->button_press(NES_B);
                     kstate[NES_B] = true;
                 }
                 break;
             }
             case SDLK_SPACE: {
                 if (!kstate[NES_START]) {
                     std::cout << "press start" << std::endl;
                     memory->button_press(NES_START);
                     ppu->kmsv1();
                     kstate[NES_START] = true;
                 }
                 break;
             }
             case SDLK_BACKSPACE: {
                 if (!kstate[NES_SELECT]) {
                     std::cout << "press select" << std::endl;
                     memory->button_press(NES_SELECT);
                     ppu->kmsv2();
                     kstate[NES_SELECT] = true;
                 }
                 break;
             }
         }
    }
    
    if (event->type == SDL_KEYUP) {
        uint64_t kup = event->key.keysym.sym;
        
        switch (kup) {
            case SDLK_UP: {
                if (kstate[NES_UP]) {
                    std::cout << "release up" << std::endl;
                    memory->button_release(NES_UP);
                    kstate[NES_UP] = false;
                }
                break;
            }
            case SDLK_DOWN: {
                if (kstate[NES_DOWN]) {
                    std::cout << "release down" << std::endl;
                    memory->button_release(NES_DOWN);
                    kstate[NES_DOWN] = false;
                }
                break;
            }
            case SDLK_RIGHT: {
                if (kstate[NES_RIGHT]) {
                    std::cout << "release right" << std::endl;
                    memory->button_release(NES_RIGHT);
                    kstate[NES_RIGHT] = false;
                }
                break;
            }
            case SDLK_LEFT: {
                if (kstate[NES_LEFT]) {
                    std::cout << "release left" << std::endl;
                    memory->button_press(NES_LEFT);
                    kstate[NES_LEFT] = false;
                }
                break;
            }
            case SDLK_z: {
                if (kstate[NES_A]) {
                    std::cout << "release A" << std::endl;
                    memory->button_press(NES_A);
                    kstate[NES_A] = false;
                }
                break;
            }
            case SDLK_x: {
                if (kstate[NES_B]) {
                    std::cout << "release B" << std::endl;
                    memory->button_press(NES_B);
                    kstate[NES_B] = false;
                }
                break;
            }
            case SDLK_SPACE: {
                if (kstate[NES_START]) {
                    std::cout << "release start" << std::endl;
                    memory->button_press(NES_START);
                    kstate[NES_START] = false;
                }
                break;
            }
            case SDLK_BACKSPACE: {
                if (kstate[NES_SELECT]) {
                    std::cout << "release select" << std::endl;
                    memory->button_press(NES_SELECT);
                    kstate[NES_SELECT] = false;
                }
                break;
            }
        }
    }
}

NES::~NES() {
    SDL_DestroyWindow(window);
    window = NULL;
    SDL_Quit();
}