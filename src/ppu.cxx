#include "ppu.hpp"

void Sprite::ff() {
    Y = 0xff;
    index = 0xff;
    attributes = 0xff;
    X = 0xff;
}

uint8_t Sprite::palette() {
    return attributes & 0x3;
}

uint8_t Sprite::priority() {
    return (attributes & 0x20) >> 5;
}

uint8_t Sprite::horizontal_flip() {
    return (attributes & 0x40) >> 6;
}

uint8_t Sprite::vertical_flip() {
    return (attributes & 0x80) >> 7;
}

uint8_t Sprite::byte(uint8_t index) {
    switch (index) {
		case 0: {
			return Y;
		}
		case 1: { 
			return index;
		}
		case 2: { 
			return attributes;
		}
		case 3: { 
			return X; 
		}
	}
}

PPU::PPU(std::shared_ptr<Mem> memory, SDL_Window* window) {
    this->memory = memory;
    this->window = window;
    
    renderer = SDL_CreateRenderer(window, -1, 0);
    screen = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STATIC, WIDTH, HEIGHT);
}

PPU::~PPU() {
    SDL_DestroyTexture(screen);
    SDL_DestroyRenderer(renderer);
}

// PPUCTRL
uint16_t PPU::get_base_nametable_addr() {
    uint8_t base = regs[0] & 0x3;
    uint16_t addr = 0x2000 + (NAMETABLE * base);
    return addr;
}

uint8_t PPU::get_vram_increment() {
    bool flag = (regs[0] >> 2) & 1;
    
    if (flag) return 32;
    else return 1;
}

uint16_t PPU::get_sprite_pattern_table_addr() {
    bool flag = (regs[0] >> 3) & 1;
    if (flag) return 0x1000;
    else return 0;
}

uint16_t PPU::get_background_pattern_table_addr() {
    bool flag = (regs[0] >> 4) & 1;
    if (flag) return 0x1000;
    else return 0;
}

uint8_t PPU::get_sprite_height() {
    bool flag = (regs[0] >> 5) & 1;
    if (flag) return 16;
    else return 8;
}

bool PPU::get_ppu_select() {
    bool flag = (regs[0] >> 6) & 1;
    return flag;
}

bool PPU::get_vblank_nmi_flag() {
    bool flag = (regs[0] >> 7) & 1;
    return flag;
}

void PPU::vram_increment() {
    uint8_t inc = get_vram_increment();
    
    std::cout << unsigned(inc) << std::endl;
    
    vram_addr += inc;
    
    if (vram_addr > 0x3fff) {
        vram_addr -= 0x3fff;
    }
}

// PPUMASK

bool PPU::get_greyscale() {
    return regs[1] & 1;
}

bool PPU::get_background_left_flag() {
    return (regs[1] >> 1) & 1;
}

bool PPU::get_sprite_left_flag() {
    return (regs[1] >> 2) & 1;
}

bool PPU::get_background_flag() {
    return (regs[1] >> 3) & 1;
}

bool PPU::get_sprite_flag() {
    return (regs[1] >> 4) & 1;
}

bool PPU::get_red_flag() {
    return (regs[1] >> 5) & 1;
}

bool PPU::get_green_flag() {
    return (regs[1] >> 6) & 1;
}

bool PPU::get_blue_flag() {
    return (regs[1] >> 7) & 1;
}

bool PPU::is_rendering_enabled() {
    return get_background_flag() || get_sprite_flag();
}

// PPUSTATUS

void PPU::set_overflow_flag(bool value) {
    regs[2] &= 0xdf;
    regs[2] |= ((uint8_t) value) << 5;
}

void PPU::set_sprite_0_hit_flag(bool value) {
    regs[2] &= 0xbf;
    regs[2] |= ((uint8_t) value) << 6;
}

void PPU::set_vblank_flag(bool value) {
    if (get_vblank_nmi_flag()) memory->set_nmi(value);
    regs[2] &= 0xef;
    regs[2] |= ((uint8_t) value) << 7;
}

// rendering

uint16_t PPU::get_tile_address() {
    uint16_t addr = 0x2000 | (vram_addr & 0x0FFF);
    return addr;
}

uint16_t PPU::get_attribute_address() {
    uint16_t addr = 0x23C0 | (vram_addr & 0x0C00) | ((vram_addr >> 4) & 0x38) | ((vram_addr >> 2) & 0x07);
    return addr;
}

// timing

void PPU::inc_cycle() {
    cycles++;
    if (cycles % 341 == 0) inc_scanline();
}

void PPU::inc_scanline() {
    current_scanline = (current_scanline + 1) % 262;
}

void PPU::ext_reg_write(uint64_t index, uint8_t value) {
    if (index >= REGS) {
        throw std::out_of_range("invalid ppu register index");
    }
    
    // scrolling register controls
    switch (index) {
        // PPUCTRL
        case 0: {
            uint16_t ba = value & 0x2;
            ba <<= 10;
            
            temp_vram_addr &= 0xf3ff;
            temp_vram_addr |= ba;
            break;
        }
        // PPUSCROLL
        case 5: {
            // $2005 first write (w is 0)
            if (!write_toggle) {
                uint8_t ba = (value & 0xf8) >> 3;
                temp_vram_addr &= 0xffe0;
                temp_vram_addr |= ba;
                
                fine_x = value & 0x7;
                
                write_toggle = 1;
            } 
            // $2005 second write (w is 1)
            else {
                uint16_t ba = ((uint16_t) (value & 0xf8)) << 2;
                
                temp_vram_addr &= 0x8c1f;
                temp_vram_addr |= ba;
                
                ba = ((uint16_t) (value & 0x7)) << 12;
                temp_vram_addr |= ba;
                
                write_toggle = 0;
            }
            
            break;
        }
        // PPUADDR
        case 6: {
            // $2006 first write (w is 0)
            if (!write_toggle) {
                uint16_t ba = (value & 0x3f) << 8;
                temp_vram_addr &= 0x40ff;
                temp_vram_addr |= ba;
                
                write_toggle = 1;
            } 
            
            // $2006 second write (w is 1)
            else {
                temp_vram_addr &= 0xff00;
                temp_vram_addr |= value;
                vram_addr = temp_vram_addr;
                write_toggle = 0;
            }
            break;
        }
        case 7: {
            if (get_vblank_nmi_flag() || !is_rendering_enabled()) {
                memory->ppu_write(vram_addr, value);
                vram_increment();
            }
            break;
        }
    }
    
    regs[2] &= ~0x1f;
    regs[2] |= (value & 0x1f);
    regs[index] = value;
}

uint8_t PPU::ext_reg_read(uint64_t index) {
    uint8_t value = regs[index];
    switch (index) {
        case 2: {
            regs[index] &= 0x7f;
            break;
        }
        case 7: {
            if (get_vblank_nmi_flag() || !is_rendering_enabled()) {
                if (vram_addr > 0x3eff) {
                    value = memory->ppu_read(vram_addr);
                    vram_increment();
                } else {
                    regs[index] = memory->ppu_read(vram_addr);
                    vram_increment();
                }
                
            }
            break;
        }
        default: {
            value = reg_latch;
            break;
        }
    }
    return value;
}

void PPU::set_oam(uint8_t byte) {
    //Sets all of OAM to the data on the corresponding input page.
    uint16_t word_addr = ((uint16_t) byte) << 8;
    for (int i = 0; i < 0xFF; i+= 4) {
        //Each sprite has 4 bytes of data. We fill the 64 sprites in.
        int sprite_index = i / 4;
        oam[sprite_index].Y = memory->mem_read(word_addr + i);
        oam[sprite_index].index = memory->mem_read(word_addr + i + 1);
        oam[sprite_index].attributes = memory->mem_read(word_addr + i + 2);
        oam[sprite_index].X = memory->mem_read(word_addr + i + 3);
    }
}

uint16_t PPU::get_vram_addr() {
    return vram_addr;
}

void PPU::inc_coarse_x() {
    if ((vram_addr & 0x1F) == 31) {
        vram_addr &= ~0x001F;
        vram_addr ^= 0x0400;
    } else {
        vram_addr++;
    }
}

void PPU::inc_fine_y() {
    if ((vram_addr & 0x7000) != 0x7000) {
        vram_addr += 0x1000;
    } else {
        vram_addr &= ~0x7000;
        uint16_t y = (vram_addr & 0x03E0) >> 5;
        if (y == 29) {
            y = 0;
            vram_addr ^= 0x0800;
        } else if (y == 31) {
            y = 0;
        } else {
            y++;
        }
        
        vram_addr = (vram_addr & ~0x03E0) | (y << 5);
    }
}

uint8_t PPU::get_coarse_x() {
    uint8_t coarse_x = vram_addr & 0x1F;
    return coarse_x;
}

uint8_t PPU::get_coarse_y() {
    uint8_t coarse_y = (vram_addr >> 5) & 0x1F;
    return coarse_y;
}

uint8_t PPU::get_nametable_index() {
    uint8_t nametable_addr = (vram_addr >> 10) & 0x3;
    return nametable_addr;
}

uint8_t PPU::get_fine_y() {
    uint8_t fine_y = (vram_addr >> 12) & 0x3;
}

// DRAWING


void PPU::decrement_sprite_counter() {
//Decrements sprite x-counter, which if 0 means the sprite is active, and begins to be displayed on the screen.
    for (int i = 0; i < SPRITES_SEC; i++) {
        if (sprite_x[i] != 0) {
            sprite_x[i]--;
        }
    }
}

void PPU::scanl_bkg() {
    bool bkg_render = get_background_flag();
    bool spr_render = get_sprite_flag();
    
    bool render = is_rendering_enabled();
    
    if (!render) {
        return;
    }
    
    uint16_t cycle = cycles % 341;
    
    if (cycle == 0) {
        // do stuff if bg + odd
        return;
    } else if ((cycle > 0 && cycle < 257) || (cycle > 320 && cycle < 337)) {
        uint8_t cycle_prog = cycle % 8;
        
        switch (cycle_prog) {
            // NT byte
            case 1: {
                uint16_t tile_addr = get_tile_address();
                nametable_byte = memory->ppu_read(tile_addr);
                break;
            }
            
            // AT byte
            case 3: {
                uint16_t attr_addr = get_attribute_address();
                attribute_byte = memory->ppu_read(attr_addr);
                break;
            }
            
            // low BG tile byte
            case 5: {
                uint16_t base_bkg_addr = get_background_pattern_table_addr();
                uint8_t fine_y = get_fine_y();
                bkg_addr = (((uint16_t) nametable_byte) << 4) + fine_y;
                low_pattern = memory->ppu_read(bkg_addr);
                
                break;
            }
            
            // high BG tile byte
            case 7: {
                bkg_addr += 8;
                high_pattern = memory->ppu_read(bkg_addr);
                
                break;
            }
            
            // add data to registers
            case 0: {
                if (cycles == 328) {
                    palette_attribute_1 = attribute_byte;
                    high_shift >>= 8;
                    low_shift >>= 8;
                } else {
                    palette_attribute_2 = attribute_byte;
                    high_shift |= ((uint16_t) high_pattern) << 8;
                    low_shift |= ((uint16_t) low_pattern) << 8;
                }
                
                if (cycles != 256) inc_coarse_x();
                else inc_fine_y();
                break;
            }
        }
    } else if (cycle == 257) {
        vram_addr &= 0xfbe0;
        vram_addr |= temp_vram_addr & ~0xfbe0;
    }
}

void PPU::scanl_spr() {
    
    // if both renders are disabled, then skip
    bool render = is_rendering_enabled();
    
    if (!render) {
        return;
    }
    
    uint16_t cycle = cycles % 341;
    
    if (cycle > 0 && cycle < 65) {
        // clear oam_sec data
        if (cycle % 8 == 0) {
            oam_sec[(cycle / 8) - 1].ff();
        }
        
        sprites_found = 0;
    } else if (cycle > 64 && cycle < 257) {
        if (cycle == 65) n = 0;
        
        if (n < 64) {

            if (cycle % 2) {
                sprite_buffer = oam[n];
            } else {
                if (sprites_found < 8) {
                    uint8_t distance = get_fine_y() - sprite_buffer.Y;
                    if (distance < 8) {
                        oam_sec[sprites_found] = sprite_buffer;
                        sprites_found++;
                    }
                    n++;
                } else {
                    while (m < 4 && n < 64) {
                        uint8_t distance = get_fine_y() - sprite_buffer.byte(m);
                        if (distance < 8) {
                            set_overflow_flag(1);
                            break;
                        } else {
                            n++;
                            m++;
                        }
                    }
                }
            }
        } 
    } else if (cycle > 256 && cycle < 321) {
        uint8_t cycle_prog = cycle % 8;
        uint8_t sprite_num = (cycle - 256 - 1) / 8;
        
        sprite_bitmap_low[sprite_num] = get_sprite_bitmap_low(oam_sec[sprite_num]);
        sprite_bitmap_high[sprite_num] = get_sprite_bitmap_high(oam_sec[sprite_num]);
        sprite_attributes[sprite_num] = oam_sec[sprite_num].attributes;
        sprite_x[sprite_num] = oam_sec[sprite_num].X;
    }
}

uint8_t PPU::get_sprite_bitmap_low(Sprite sprite) {
    uint16_t pattern_addr = get_sprite_pattern_table_addr();
    pattern_addr += sprite.index << 3;
    pattern_addr += (current_scanline + 1) % 8;
    
    uint8_t bitmap_low = memory->ppu_read(pattern_addr);
    return bitmap_low;
}

uint8_t PPU::get_sprite_bitmap_high(Sprite sprite) {
    uint16_t pattern_addr = get_sprite_pattern_table_addr();
    pattern_addr += sprite.index << 3;
    pattern_addr += (current_scanline + 1) % 8;
    pattern_addr += 8;
    
    uint8_t bitmap_high = memory->ppu_read(pattern_addr);
    return bitmap_high;
}

void PPU::render_pixel() {
    uint16_t cycle = cycles % 341;
    
    if (cycle > 0 && cycle < 257) {
        uint8_t background_pixel = get_background_pixel();
        uint8_t sprite_pixel = get_sprite_pixel();
                    
        if (sprite_foreground || background_pixel == 0) {
            pixel_array[current_scanline][cycle] = sprite_pixel;
        } else {
            pixel_array[current_scanline][cycle] = background_pixel;
        }
        
        low_shift >>= 1;
        high_shift >>= 1;
    }
}

uint8_t PPU::get_background_pixel() {
    uint8_t fx = (fine_x + cycles % 341) % 8;
    uint8_t color_byte = ((low_shift >> fx) & 0x1) + (((high_shift >> fx) & 0x1) << 1);
    bool even_x = (get_coarse_x() / 16) % 2;
    bool even_y = (current_scanline / 16) % 2;
    uint8_t shift = (((uint8_t) even_x) + (((uint8_t) even_y) << 1)) << 1;
    uint8_t color_set = (attribute_byte >> shift) & 0x3;
    
    uint16_t color_addr = 0x3f00 + color_byte + (color_set << 2);
    uint8_t palette_color = memory->ppu_read(color_addr);
    
    return palette_color;
}

uint8_t PPU::get_sprite_pixel() {
    //This function gets the next sprite pixel to be used for comparison with the background pixel when deciding the next pixel to display.
    uint8_t return_pixel;
    for (int i = 0; i < SPRITES_SEC; i++) {
        uint8_t color = (sprite_bitmap_low[i] >> 7) + (sprite_bitmap_high[i] >> 7) * 2;
        if (sprite_x[i] == 0) {
            //Shifts sprite bitmaps
            sprite_bitmap_low[i] = sprite_bitmap_low[i] << 1;
            sprite_bitmap_high[i] = sprite_bitmap_high[i] << 1;	  
            if (color != 0) {
                //Sprite must be active and must have a non-transparent pixel
          	  	return_pixel = memory->ppu_read(0x3F10 + 4 * (sprite_attributes[i] % 4) + color);
	          	sprite_foreground = (((sprite_attributes[i] >> 5) & 0x1) == 1);
                return return_pixel;
	        }
        }	   
    }   
}

void PPU::execute() {
    //Our main cycle execution function for PPU. Every time this is called, a cycle of PPU is executed.
    bool render = is_rendering_enabled();
    if (current_scanline >= 0 && current_scanline < 240) { 
        if (render) {
            render_pixel();
            decrement_sprite_counter();
            scanl_bkg();
            scanl_spr();
        }
        
    } else if (current_scanline == 261) {
        //Pre-render scanline fills the 2 16-bit background tile registers and sets VBLANK and sprite 0 hit flags and sprite overflow flags to 0.
        if (cycles % 341 == 1) {
            set_vblank_flag(0);
            set_sprite_0_hit_flag(0);
            set_overflow_flag(0);
        }
        
        
        if (render) {
            scanl_bkg();
        }
        
        if (cycles > 279 && cycles < 305 && render) {
            // set vertical bits
            vram_addr &= ~0x7be0; 
            vram_addr |= temp_vram_addr & 0x7be0; 
        }
        
        if (cycles % 341 == 340) {
            display();
        }
        
    } else if (current_scanline == 241) {
        if (cycles % 341 == 1)
            // Cycle 1 on 241st scanline sets VBLANK flag to 1.
            set_vblank_flag(1);
    }
    
    
    inc_cycle();
    
    uint64_t cpu_clock = memory->get_cpu_cycle();
    
    if (cpu_clock < 29659) {
        clear_writes();
    }        
}

void PPU::clear_writes() {
    regs[0] = 0;
    regs[1] = 0;
    regs[5] = 0;
    regs[6] = 0;
    regs[7] = 0;
    temp_vram_addr = 0;
    fine_x = 0;
}

void PPU::display() {
    //This function uses the SDL2 library to display the pixel array in a window.
    uint32_t* pixels = get_pixel_array();
    SDL_UpdateTexture(screen, NULL, pixels, WIDTH * sizeof(uint32_t));
    
    SDL_RenderClear(renderer);
    SDL_RenderCopy(renderer, screen, NULL, NULL);
    SDL_RenderPresent(renderer);
}

uint32_t* PPU::get_pixel_array() {
    uint32_t* pixels = new uint32_t[WIDTH * HEIGHT];
    for (int Y = 0; Y < HEIGHT; Y++) { // Y
        for (int X = 0; X < WIDTH; X++) { // X
            uint8_t val = pixel_array[Y][X];
            pixels[ADDR(X,Y)] = convert32(val);
        }
    }
    
    return pixels;
}

uint32_t PPU::convert32(uint8_t value) {
    value &= 0x3f;
    uint32_t color = color_map[value];
    // because I'm dumb
    color >>= 8;
    color += 0xFF000000;
    return color;
}

std::string PPU::debug() {
    std::stringstream buffer;
    
    buffer << "PPU: " << std::setw(3) << cycles % 341 << ", " << std::setw(3) << current_scanline;
    
    return buffer.str();
}

// render nametable
void PPU::kmsv1() {
    SDL_Window* nt_window = SDL_CreateWindow("NAMETABLE", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, WIDTH, HEIGHT, SDL_WINDOW_SHOWN);
    if (nt_window == NULL) {
        throw std::runtime_error(SDL_GetError());
    }
    
    SDL_Renderer* nt_draw = SDL_CreateRenderer(nt_window, -1, 0);
    SDL_Texture* nt_screen = SDL_CreateTexture(nt_draw, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STATIC, WIDTH, HEIGHT);
    uint32_t* pixels = new uint32_t[WIDTH * HEIGHT];
    
    std::array<uint8_t, NAMETABLE> nametable = memory->get_nametable(0);
    
    std::array<uint8_t, PATTERN_TABLE> left = memory->get_pattern_table(0);
    std::array<uint8_t, PATTERN_TABLE> right = memory->get_pattern_table(0);
    
    std::array<std::array<uint8_t, PALETTE>, 4> palettes = memory->get_back_palettes();
    
    uint8_t univ_color = memory->get_univ_back_color();
    
    for (int y = 0; y < HEIGHT; y++) {
        for (int x = 0; x < WIDTH; x++) {
            uint16_t nt_index = (x + y * WIDTH) / 8;
            uint8_t nt_byte = nametable[nt_index];
            
            uint8_t attr_y_offset = y / 32;
            uint8_t attr_x_offset = x / 32;
            
            uint8_t attr_byte = nametable[960 + attr_x_offset + attr_y_offset * 8];
            
            bool even_x = (x / 16) % 2;
            bool even_y = (y / 16) % 2;
            uint8_t shift = (((uint8_t) even_x) + (((uint8_t) even_y) << 1)) << 1;
            
            uint8_t color_set = (attr_byte >> shift) & 0x3;
            
            uint8_t pt_index = (((uint16_t) nt_byte) << 4 + y % 8);
            uint8_t low_byte;
            uint8_t high_byte;
            
            if (pt_index < 0x1000) {
                low_byte = left[pt_index];
                high_byte = left[pt_index + 8];
            } else {
                low_byte = right[pt_index];
                high_byte = right[pt_index + 8];
            }
            
            uint8_t color_index = ((low_byte >> (7 - x % 8)) & 1) + (((high_byte >> (7 - x % 8)) & 1) << 1);
            
            uint8_t color_8;
            
            if (color_index == 0) {
                color_8 = univ_color;
            } else {
                color_8 = palettes[color_set][color_index - 1];
            }
            
            uint32_t color_32 = convert32(color_8);
            pixels[ADDR(x,y)] = color_32;
        }
    }
    
    SDL_UpdateTexture(nt_screen, NULL, pixels, WIDTH * sizeof(uint32_t));
    
    SDL_RenderClear(nt_draw);
    SDL_RenderCopy(nt_draw, nt_screen, NULL, NULL);
    SDL_RenderPresent(nt_draw);
}

// render pattern table
void PPU::kmsv2() {
    SDL_Window* pt_window = SDL_CreateWindow("PATTERN TABLE", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, WIDTH, HEIGHT, SDL_WINDOW_SHOWN);
    if (pt_window == NULL) {
        throw std::runtime_error(SDL_GetError());
    }
    
    SDL_Renderer* pt_draw = SDL_CreateRenderer(pt_window, -1, 0);
    SDL_Texture* pt_screen = SDL_CreateTexture(pt_draw, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STATIC, WIDTH, HEIGHT);
    uint32_t* pixels = new uint32_t[WIDTH * HEIGHT];
    
    std::array<uint8_t, PATTERN_TABLE> left = memory->get_pattern_table(0);
    std::array<uint8_t, PATTERN_TABLE> right = memory->get_pattern_table(1);
    
    std::array<std::array<uint8_t, PALETTE>, 4> palettes = memory->get_back_palettes();
    uint8_t univ_color = memory->get_univ_back_color();
    
    for (int y = 0; y < 16; y++) {
        
        for (int x = 0; x < 16; x++) {
            uint16_t tile_index = (y << 8) + (x << 4);
            
            for (int fy = 0; fy < 8; fy++) {
                uint16_t actual_index = tile_index + fy;
                
                uint8_t left_low = left[actual_index];
                uint8_t left_high = left[actual_index + 8];
                
                uint8_t right_low = right[actual_index];
                uint8_t right_high = right[actual_index + 8];
                
                for (int fx = 0; fx < 8; fx++) {
                    // draw left
                    uint8_t left_color_index = ((left_low >> (7 - fx)) & 1) + (((left_high >> (7 - fx)) & 1) << 1);
                    uint8_t left_color_8;
            
                    if (left_color_index == 0) {
                        left_color_8 = univ_color;
                    } else {
                        left_color_8 = palettes[0][left_color_index - 1];
                    }
                    
                    uint32_t left_color_32 = convert32(left_color_8);
                    
                    // draw right
                    uint8_t right_color_index = ((right_low >> (7 - fx)) & 1) + (((right_high >> (7 - fx)) & 1) << 1);
                    uint8_t right_color_8;
            
                    if (right_color_index == 0) {
                        right_color_8 = univ_color;
                    } else {
                        right_color_8 = palettes[0][right_color_index - 1];
                    }
                    
                    uint32_t right_color_32 = convert32(right_color_8);
                    
                    // put
                    pixels[(y * 8 + fy) * WIDTH + x * 8 + fx] = left_color_32;
                    pixels[(y * 8 + fy) * WIDTH + x * 8 + fx + 128] = right_color_32;
                }
            }
        }
    }
    
    SDL_UpdateTexture(pt_screen, NULL, pixels, WIDTH * sizeof(uint32_t));
    
    SDL_RenderClear(pt_draw);
    SDL_RenderCopy(pt_draw, pt_screen, NULL, NULL);
    SDL_RenderPresent(pt_draw);
}
