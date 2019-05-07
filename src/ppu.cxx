#include "ppu.hpp"

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

PPU::PPU(std::shared_ptr<Mem> memory) {
    this->memory = memory;
}

void PPU::set_reg(uint64_t index, uint8_t value) {
    if (index >= REGS) {
        throw std::out_of_range("invalid ppu register index");
    }
    
    regs[index] = value;
}

uint8_t PPU::read_reg(uint64_t index) {
    return regs[index];
}

void PPU::set_oam(uint8_t byte) {
//Sets all of OAM to the data on the corresponding input page.
    uint8_t word_addr = byte << 8;
    for (int i = 0; i < 0xFF; i+= 4) {
        //Each sprite has 4 bytes of data. We fill the 64 sprites in.
        int sprite_index = i / 4;
        oam[sprite_index].Y = memory->mem_read(word_addr);
        oam[sprite_index].index = memory->mem_read(word_addr + 1);
        oam[sprite_index].attributes = memory->mem_read(word_addr + 2);
        oam[sprite_index].X = memory->mem_read(word_addr + 3);
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

void PPU::inc_vram_addr(uint8_t type) {
    if (type == 0) {
	vram_addr += 1;
    } else {
	vram_addr += 32;
    }
}

void PPU::set_scroll_coord(uint8_t value) {
    if (!addr_latch) {
        uint8_t coarse_x = value / 8;
        fine_x = value % 8;
        temp_vram_addr = (temp_vram_addr >> 5) << 5 + coarse_x;
    } else {
        uint8_t coarse_y = value / 8;
        temp_vram_addr = (temp_vram_addr & ~0x03E0) + (coarse_y << 5);
        temp_vram_addr = (temp_vram_addr & ~0x7000) + ((value % 8) << 12);
    }

    addr_latch = !addr_latch;
}

uint8_t PPU::get_buffer() {
    return read_buffer;
}

void PPU::update_buffer() {
    read_buffer = memory->ppu_read(vram_addr);
}

void PPU::reset_addr_latch() {
//Latch toggle function
     addr_latch = false;
}

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
//This function fills in the next pixel in the grid.
    uint8_t color = (background_bitmap_1 >> (8 + fine_x) & 0x1) << 1 + ((background_bitmap_1 >> fine_x) & 0x1);
    uint8_t palette_attribute;
    uint8_t coarse_x = vram_addr % 32;
    uint8_t coarse_y = (vram_addr >> 5) % 32;

    switch (coarse_y * 2 + coarse_x) {
        //This determines which quadrant the address is in, and selects the 2-bit appropriate attribute from the byte.
        case 0: {
            palette_attribute = attribute_byte_1 & 0x3;
            break;
        }
        case 1: {
            palette_attribute = (attribute_byte_1 >> 2) & 0x3;
            break;
        }
        case 2: {
            palette_attribute = (attribute_byte_1 >> 4) & 0x3;
            break;
        }
        case 3: {
            palette_attribute = (attribute_byte_1 >> 6) & 0x3;
            break;
        }
    }
	
    uint8_t background_pixel = memory->ppu_read(0x3F00 + 4 * palette_attribute + color);
    uint8_t sprite_pixel = get_sprite_pixel();	

    //Determines whether sprite or background pixel is displayed	
    if (sprite_foreground || background_pixel == 0) {
        pixel_array[current_scanline][cycle_mod_341] = sprite_pixel;
    } else {
        pixel_array[current_scanline][cycle_mod_341] = background_pixel;
    }

    if (sprite_pixel != 0 && background_pixel != 0) {
        //Sets sprite 0 hit flag to 1.
        regs[2] = regs[2] | 0x40;
    }
}

void PPU::fill_sprite_bitmaps() {
    //This function is responsible for filling the bitmaps for the sprites to be used on the next scanline.

    uint8_t fine_y = ((vram_addr >> 12) + 1) % 8;
    uint8_t sprite_num = (cycle_mod_341 - 257) % 257; 
    bool ver_flip = (sprite_attribute_latch >> 7) == 1;
    bool hor_flip = (sprite_attribute_latch >> 6) == 1;
    uint8_t temp_bitmap;
    //Left pattern table or right pattern table
    uint8_t sprite_pattern_offset = ((regs[0] >> 3) & 1 == 1) ? 0x1000 : 0;

    switch (cycle_mod_8) {
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


void PPU::fill_first_tiles() {
	if (cycle_mod_341 == 321) {
        //Nasty calculations to adjust vram_address with the new scanline.
		uint16_t temp_coarse_y = (temp_vram_addr >> 5) % 32;
		uint16_t temp_fine_y = (temp_vram_addr >> 12) % 8;
		uint16_t y_offset;
		if (current_scanline == 261) {
		      y_offset = temp_fine_y;
	    } else {
		      y_offset = temp_fine_y + current_scanline;
		}

		if (temp_coarse_y * 8 + y_offset >= 240) {
			//This changes the nametable of the current VRAM address.
			vram_addr = temp_vram_addr ^ 0x0800;
		}

		
		uint16_t new_fine_y = (y_offset) % 8;
		uint16_t new_coarse_y = ((temp_coarse_y * 8 + y_offset) % 240) / 8;
	    vram_addr = (vram_addr & 0x0FFF) | (new_fine_y << 12);
		vram_addr = (vram_addr & 0x7C1F) | (new_coarse_y << 5);
		
		//Sets coarse x of vram addr
		vram_addr = (vram_addr >> 5) << 5 + (temp_vram_addr % 32);	
	}

	//Now we are ready to fetch the first two tiles
	uint16_t nametable_num = (vram_addr >> 10) & 0x3;	
	uint16_t nametable_addr = (nametable_num << 10) + 0x2000;
	uint8_t coarse_x = vram_addr % 32;
	uint8_t coarse_y = (vram_addr >> 5) % 32;

	
	if (cycle_mod_341 >= 322 && cycle_mod_341 <= 336) {
        uint8_t back_pattern_offset = (((regs[0] >> 4) & 0x1) == 1) ? 0x1000 : 0x0;	
       		switch (cycle_mod_8) {
                case 2: {
                    //Reads nametable byte
                    uint16_t byte_addr = nametable_addr + 8 * coarse_y + coarse_x;
                    nametable_byte = memory->ppu_read(byte_addr);
                    break;
                }  
                case 4: {
			        //Reads attribute byte
                    uint8_t att_y = coarse_y / 4;
                    uint8_t att_x = coarse_x / 4;
                    attribute_byte = memory->ppu_read(nametable_addr + 960 + att_x + att_y * 8);
                    break;
                }
                case 6: {
                    uint8_t fine_y = (vram_addr >> 12) % 8;
                    if (cycle_mod_341 == 326) {
                        //First 8 cycles fill first bitmap, and second 8 cycles fill second bitmap
                        background_bitmap_1 = 0;
                        background_bitmap_1 |= memory->ppu_read(back_pattern_offset + (nametable_byte << 4) + fine_y);
                    }
    
                    else {
                        background_bitmap_2 = 0;
                        background_bitmap_2 |= memory->ppu_read(back_pattern_offset + nametable_byte << 4 + fine_y);
                    }
                    break;
			 }
                case 0: {
                    uint8_t fine_y = (vram_addr >> 12) % 8;
                    if (cycle_mod_341 == 328) {
                        background_bitmap_1 |= memory->ppu_read(back_pattern_offset + (nametable_byte << 4) + (8 + fine_y));
                    } else {
                        background_bitmap_2 |= memory->ppu_read(back_pattern_offset + (nametable_byte << 4) + (8 + fine_y));
                    }
                
                    
                    if ((vram_addr & 0x001F) == 31) {
                        //If coarse x is 31, the nametable shifts and coarse x is set to 0
                        vram_addr &= !0x001F;
                        vram_addr ^= 0x0400;
                    } else {
                        //Otherwise, coarse x is just incremented by 1
                        vram_addr += 1;
                    }
                    break;
                }

		 }
	}
}

void PPU::background_eval() {
    //This function handles background-related operations.

    //Cycle 0 just idles
    if (cycle_mod_341 >= 1 && cycle_mod_341 <= 256) {
        //These cycles are responsible for writing the background scanline of pixels.
    	 
        //Nametable number being used(0 - 3)
        uint16_t nametable_index = (vram_addr >> 10) & 0x3;	
        //Address of nametable being used
        uint16_t nametable_addr = (nametable_index << 10) + 0x2000;
        //X and Y coords of tile corresponding to current location
        uint8_t coarse_x = vram_addr % 32;
        uint8_t coarse_y = (vram_addr >> 5) % 32;
            
        fill_next_pixel();
        decrement_sprite_counter();
    
        //Fine x is incremented, and if it is, the tile registers are shifted
        fine_x = (fine_x + 1) % 8;
        if (fine_x == 0) {
            attribute_byte_1 = attribute_byte_2;
            background_bitmap_1 = background_bitmap_2;
        }
    
        uint8_t back_pattern_offset = ((regs[0] >> 4) & 0x1 == 1) ? 0x1000 : 0;
        //This block of code details what tile data is fetched from PPU memory every two cycles.
        if (cycle_mod_8 == 1)  {
            //Nametable byte.
            uint16_t byte_addr = nametable_addr + coarse_x + coarse_y * 32;
            nametable_byte = memory->ppu_read(byte_addr);
        } else if (cycle_mod_8 == 3) {
            //Attrbute byte.
            uint8_t att_y = coarse_y / 4;
            uint8_t att_x = coarse_x / 4;
            attribute_byte = memory->ppu_read(nametable_addr + 960 + att_x + 8 * att_y);
        } else if (cycle_mod_8 == 5) {
            //Tile bitmap low
            uint8_t offset = (vram_addr >> 12) % 8;
            uint16_t tile_address = back_pattern_offset + (nametable_byte << 4) + offset;
            bitmap_latch = memory->ppu_read(tile_address);	
        } else if (cycle_mod_8 == 7) {
            //Tile bitmap high.
            uint8_t fine_y = (vram_addr >> 12) % 8;
            uint16_t tile_address = back_pattern_offset + (nametable_byte << 4) + 8 + fine_y;
            bitmap_latch += memory->ppu_read(tile_address) << 8;	
        
            attribute_byte_2 = attribute_byte;
            background_bitmap_2 = bitmap_latch; 
            //After we fetch all the necessary variables, we draw the next 8 pixels on the row		 
    
            //Does operations on coarse_x
            if (vram_addr & 0x001F == 31) {
                vram_addr &= ~0x001F;
                vram_addr ^= 0x0400; 
            } else {
                vram_addr += 1;
            }
        }
    } else if (cycle_mod_341 >= 257 && cycle_mod_341 <= 320) {
        fill_sprite_bitmaps();
    } else if (cycle_mod_341 >= 321 && cycle_mod_341 <= 336) {
        fill_first_tiles();
    } else if (cycle_mod_341 >= 337 && cycle_mod_341 <= 340) {
	//These cycles just get the nametable byte for the 3rd tile in the next scanline, which is pointless since it'll be retrieved again in the next scanline process.
	//Will implement if there is time.
    }
}

uint8_t PPU::sprite_byte(Sprite sprite, uint8_t index) {
	switch (index) {
		case 0: {
			return sprite.Y;
		}
		case 1: { 
			return sprite.index;
		}
		case 2: { 
			return sprite.attributes;
		}
		case 3: { 
			return sprite.X; 
		}
	}
}

void PPU::sprite_eval() {
//This function is responsible for placing sprites into secondary OAM and filling the sprite data latches to be used in sprite evaluation for the next scanline.
    if (cycle_mod_341 <= 1 && cycle_mod_341 >= 64) {
        switch (cycle_mod_8) {
            //Even cycles write 0xFF to secondary OAM
            case 2: {
                oam_sec[(cycle_mod_341 - 1) / 8].Y = 0xFF;
                break;
            }
    
            case 4: {
                oam_sec[(cycle_mod_341 - 1) / 8].index = 0xFF;
                break;
            }
    
            case 6: {
                oam_sec[(cycle_mod_341 - 1) / 8].attributes = 0xFF;
                break;
            }
    
            case 0: {
                oam_sec[(cycle_mod_341 - 1) / 8].X = 0xFF;
                break;
            }
    
            default: {//Odd cycles read from primary OAM, and this is just to implement that
                primary_oam_byte = 0xFF;
            }
            
            oam_sec_full = false;	
            in_range = false;
            oam_sec_index = 0;
        }
        return;
    
        m = 0;
        n = 0;
    } else if (cycle_mod_341 >= 65 && cycle_mod_341 <= 256) {
        //These cycles fill up secondary OAM with new data
        if (cycle_mod_8 % 2 == 0) {
            if (in_range && oam_sec_full) {
                //Set sprite overflow flag and read next 3 entries of OAM
                regs[2] = regs[2] | 0x20;
            } else if (!in_range && oam_sec_full) {
                //If not in range and secondary OAM is full		  
            } else if (in_range) {
            //If in range and secondary OAM is not full
                if (m == 1) {
                    oam_sec[oam_sec_index].index = primary_oam_byte;
                }	  
                else if (m == 2) {
                    oam_sec[oam_sec_index].attributes = primary_oam_byte;
                }
                
                else if (m == 3) {
                    //Set the last byte
                    oam_sec[oam_sec_index].X = primary_oam_byte;
                        //Then reset other variables
                    in_range = false;
                    //Increment all counters
                    oam_sec_index += 1;
                    if (oam_sec_index == 8) {
                    oam_sec_full = true;
                    }
                    
                    n = (n + 1) % 64;
                }
                m = (m + 1) % 4;
            } else if ((primary_oam_byte - current_scanline >= -7) && (primary_oam_byte - current_scanline <= 7) && !in_range) {
                //If sprite to be found is in range and secondary OAM is not full, indicate that it is in-range and set counters as appropriate
                oam_sec[oam_sec_index].Y = primary_oam_byte;
                in_range = true;
                m++;
            } else {
                //Just increment if sprite to be found is not in range
                n++;
            }
        } else {
            primary_oam_byte = sprite_byte(oam[n], m);
            return;
        }
    } else if (cycle_mod_341 >= 257 && cycle_mod_341 <= 320) {
	 
        uint8_t sprite_num = (cycle_mod_341 - 257) / 8;
        switch (cycle_mod_8) {
            case 0: {
                sprite_y_latch = oam_sec[sprite_num].Y;
                break;	
            }	
    
            case 1: {
                sprite_tile_latch = oam_sec[sprite_num].index;
                break;
            }
    
            case 2: {
                sprite_attribute_latch = oam_sec[sprite_num].attributes;
                break;
            }
    
            case 3: {
                sprite_x_latch = oam_sec[sprite_num].X;
                break;
            }
    
            default: {//The sprite evaluation process just does this to fill up the cycles.
                sprite_x_latch = oam_sec[sprite_num].X;
                break;
            }
        }
    }
}

void PPU::execute() {
//Our main cycle execution function for PPU. Every time this is called, a cycle of PPU is executed.
    if (current_scanline >= 0 && current_scanline < 240) { 
    	 background_eval();
    	 sprite_eval();
    } else if (current_scanline == 261) {
        //Pre-render scanline fills the 2 16-bit background tile registers and sets VBLANK and sprite 0 hit flags and sprite overflow flags to 0.
	 
        if (cycle_mod_341 == 0) {
            regs[2] = regs[2] & ~0x80;
            regs[2] = regs[2] & ~0x40;
            regs[2] = regs[2] & ~0x20;
            vram_addr = temp_vram_addr;
            fine_x = temp_fine_x;
        }
        fill_first_tiles();
    } else if (current_scanline == 241) {
        //240th scanline sets VBLANK flag to 1.
        regs[2] = regs[2] | 0x80;
    }
    
    cycle_mod_8 = (cycle_mod_8 + 1) % 8;
    cycle_mod_341 = (cycle_mod_341 + 1) % 341;
    if (cycle_mod_341 == 0) {
        //Important to reset cycle_mod_8 for the new scanline so calculations of cycles aren't messed up
        cycle_mod_8 = 0;
        current_scanline = (current_scanline + 1) % 262;
    }

}

void PPU::display() {
//This function uses the SDL2 library to display the pixel array in a window.
    
}


