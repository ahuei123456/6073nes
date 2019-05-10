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
            vram_addr += get_vram_increment();
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

void PPU::set_vram_addr(uint8_t value) {
//Setting the new vram address shifts left the lowest 8 bits and then adds a byte value.
    if (!addr_latch) {
        vram_addr = ((uint16_t) value << 8) + (vram_addr % 0xFF); 
    } else {
        vram_addr = ((vram_addr >> 8) << 8) + value;
    }

    addr_latch = !addr_latch;
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

void PPU::fill_next_pixel() {
    uint16_t prog = cycles % 341;
    //This function fills in the next pixel in the grid.
    uint8_t color = (background_bitmap_1 >> (8 + fine_x) & 0x1) << 1 + ((background_bitmap_1 >> fine_x) & 0x1);
    uint8_t palette_attribute;
    uint8_t coarse_x = get_coarse_x();
    uint8_t coarse_y = get_coarse_y();

    switch ((coarse_y * 2 + coarse_x) % 4) {
        //This determines which quadrant the address is in, and selects the 2-bit appropriate attribute from the byte.
        case 0: {
            palette_attribute = palette_attribute_1 & 0x3;
            break;
        }
        case 1: {
            palette_attribute = (palette_attribute_1 >> 2) & 0x3;
            break;
        }
        case 2: {
            palette_attribute = (palette_attribute_1 >> 4) & 0x3;
            break;
        }
        case 3: {
            palette_attribute = (palette_attribute_1 >> 6) & 0x3;
            break;
        }
    }
	
    uint8_t background_pixel = memory->ppu_read(0x3F00 + 4 * palette_attribute + color);
    uint8_t sprite_pixel = get_sprite_pixel();	

    //Determines whether sprite or background pixel is displayed	
    if (sprite_foreground || background_pixel == 0) {
        pixel_array[current_scanline][prog] = sprite_pixel;
    } else {
        pixel_array[current_scanline][prog] = background_pixel;
    }

    if (sprite_pixel != 0 && background_pixel != 0) {
        //Sets sprite 0 hit flag to 1.
        set_sprite_0_hit_flag(1);
    }
}

void PPU::fill_sprite_bitmaps() {
    //This function is responsible for filling the bitmaps for the sprites to be used on the next scanline.
    
    uint16_t prog = cycles % 341;
    uint8_t fetch_prog = prog % 8;
    
    uint8_t fine_y = ((vram_addr >> 12) + 1) % 8;
    uint8_t sprite_num = (prog - 257) % 257; 
    bool ver_flip = (sprite_attribute_latch >> 7) == 1;
    bool hor_flip = (sprite_attribute_latch >> 6) == 1;
    uint8_t temp_bitmap;
    //Left pattern table or right pattern table
    uint8_t sprite_pattern_offset = ((regs[0] >> 3) & 1 == 1) ? 0x1000 : 0;

    switch (fetch_prog) {
        case 5: {
			//Fetches low sprite bitmap
			sprite_bitmap_low[sprite_num] = 0;
			
            //Fetches the vertical flipped bitmap if appropriate
			if (ver_flip) {
			    temp_bitmap = memory->ppu_read(sprite_pattern_offset + (sprite_tile_latch << 4) + 7 - fine_y);
			} else {
			    temp_bitmap = memory->ppu_read(sprite_pattern_offset + (sprite_tile_latch << 4) + fine_y);
			}

			if (hor_flip) {
                //Flips the 8-bit quantity if appropriate
			    for (int i = 0; i < 8; i++) {
                    sprite_bitmap_low[sprite_num] += (temp_bitmap >> (7 - i)) << i;
			    }
			} else {
			     sprite_bitmap_low[sprite_num] = temp_bitmap;
			}
			break;
		}

		case 7: {
			//Fetches high sprite bitmap
			sprite_bitmap_high[sprite_num] = 0;
			if (ver_flip) {
				temp_bitmap = memory->ppu_read(sprite_pattern_offset + (sprite_tile_latch << 4) + 7 - fine_y);
			} else {
				temp_bitmap = memory->ppu_read(sprite_pattern_offset + (sprite_tile_latch << 4) + fine_y);
			}

			if (hor_flip) {
			    for (int i = 0; i < 8; i++) {
					sprite_bitmap_high[sprite_num] += (temp_bitmap >> (7 - i)) << i;
				}
			} else {
				sprite_bitmap_high[sprite_num] = temp_bitmap;
			}
		}
	}
}

void PPU::scanl_bkg() {
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
            
            // add data to pipeline
            case 0: {
                for (int i = 0; i < 8; i++) {
                    uint8_t color_byte = ((low_pattern >> (7 - i)) & 0x1) + (((high_pattern >> (7 - i)) & 0x1) << 1);
                    bool even_x = (get_coarse_x() / 16) % 2;
                    bool even_y = (current_scanline / 16) % 2;
                    uint8_t shift = (((uint8_t) even_x) + (((uint8_t) even_y) << 1)) << 1;
                    uint8_t color_set = (attribute_byte >> shift) & 0x3;
                    
                    uint16_t color_addr = 0x3f00 + color_byte + (color_set << 2);
                    uint8_t palette_color = memory->ppu_read(color_addr);
                    
                    pipeline.push(palette_color);
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
    bool bkg_render = get_background_flag();
    bool spr_render = get_sprite_flag();
    
    if (!(bkg_render || spr_render)) {
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
        if (pipeline.size() > 0) {
            uint8_t pixel = pipeline.front();
            pipeline.pop();
            
            pixel_array[current_scanline][cycle - 1] = pixel;
        }
    }
}

void PPU::execute() {
    //Our main cycle execution function for PPU. Every time this is called, a cycle of PPU is executed.
    
    if (current_scanline >= 0 && current_scanline < 240) { 
        bool bkg_render = get_background_flag();
        bool spr_render = get_sprite_flag();
    
        if (bkg_render || spr_render) {
            scanl_bkg();
            scanl_spr();
            render_pixel();
        }
        
    } else if (current_scanline == 261) {
        //Pre-render scanline fills the 2 16-bit background tile registers and sets VBLANK and sprite 0 hit flags and sprite overflow flags to 0.
        if (cycles % 341 == 1) {
            set_vblank_flag(0);
            set_sprite_0_hit_flag(0);
            set_overflow_flag(0);
        }
        scanl_bkg();
        
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
