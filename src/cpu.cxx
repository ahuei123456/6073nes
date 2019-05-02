#include "cpu.hpp"

CPU::CPU(std::shared_ptr<Mem> memory) {
    this->memory = memory;
    
    reg_pc = memory->reset_vector();
}

void CPU::execute() {
    uint16_t opcode = memory->mem_read(reg_pc);
    
    switch (opcode) {
        case LDA_I: {
            uint16_t operand = memory->mem_read(reg_pc + 1);
	    reg_ac = (uint8_t) operand;
            if(reg_ac & 0x80 == 0)
		set_negative(false);
    	    else
		set_negative(true);
    	    if(reg_ac == 0x0)
    		set_zero(true);
    	    else 
		set_zero(false);
            break;
        }
	case LDA_Z: {
            uint16_t operand = memory->mem_read(reg_pc + 1);
            reg_ac = (uint8_t) memory->mem_read(operand);
            if(reg_ac & 0x80 == 0)
		set_negative(false);
    	    else
		set_negative(true);
    	    if(reg_ac == 0x0)
    		set_zero(true);
    	    else 
		set_zero(false);
            break;
        }
	case LDA_ZX: {
            uint16_t operand = memory->mem_read(reg_pc + 1);
            reg_ac = (uint8_t) memory->mem_read(operand + reg_x);
            if(reg_ac & 0x80 == 0)
		set_negative(false);
    	    else
		set_negative(true);
    	    if(reg_ac == 0x0)
    		set_zero(true);
    	    else 
		set_zero(false);
            break;
        }
	case LDA_A: {
            uint16_t operand = memory->mem_read2(reg_pc + 1);
            reg_ac = (uint8_t) memory->mem_read(operand);
            if(reg_ac & 0x80 == 0)
		set_negative(false);
    	    else
		set_negative(true);
    	    if(reg_ac == 0x0)
    		set_zero(true);
    	    else 
		set_zero(false);
	    break;
	}
	case LDA_AX: {
            uint16_t operand = memory->mem_read2(reg_pc + 1);
            reg_ac = (uint8_t) memory->mem_read(operand + reg_x);
            if(reg_ac & 0x80 == 0)
		set_negative(false);
    	    else
		set_negative(true);
    	    if(reg_ac == 0x0)
    		set_zero(true);
    	    else 
		set_zero(false);
            break;
        }
	case LDA_AY: {
            uint16_t operand = memory->mem_read2(reg_pc + 1);
            reg_ac = (uint8_t) memory->mem_read(operand + reg_y);
            if(reg_ac & 0x80 == 0)
		set_negative(false);
    	    else
		set_negative(true);
    	    if(reg_ac == 0x0)
    		set_zero(true);
    	    else 
		set_zero(false);
            break;
        }
	case LDA_IX: {
            uint16_t operand = memory->mem_read(reg_pc + 1);
            reg_ac = (uint8_t) memory->mem_read(memory->mem_read2(operand + reg_x));
            if(reg_ac & 0x80 == 0)
		set_negative(false);
    	    else
		set_negative(true);
    	    if(reg_ac == 0x0)
    		set_zero(true);
    	    else 
		set_zero(false);
            break;
        }
	case LDA_IY: {
            uint16_t operand = memory->mem_read(reg_pc + 1);
            reg_ac = (uint8_t) memory->mem_read((memory->mem_read2(operand) + reg_y);
            if(reg_ac & 0x80 == 0)
		set_negative(false);
    	    else
		set_negative(true);
    	    if(reg_ac == 0x0)
    		set_zero(true);
    	    else 
		set_zero(false);
            break;
        }
        case LDX_I: {
            uint16_t operand = memory->mem_read(reg_pc + 1);
	    reg_x = (uint8_t) operand;
            if(reg_x & 0x80 == 0)
		set_negative(false);
    	    else
		set_negative(true);
    	    if(reg_x == 0x0)
    		set_zero(true);
    	    else 
		set_zero(false);
            break;
        }
	case LDX_Z: {
            uint16_t operand = memory->mem_read(reg_pc + 1);
            reg_x = (uint8_t) memory->mem_read(operand);
            if(reg_x & 0x80 == 0)
		set_negative(false);
    	    else
		set_negative(true);
    	    if(reg_x == 0x0)
    		set_zero(true);
    	    else 
		set_zero(false);
            break;
        }
	case LDX_ZY: {
            uint16_t operand = memory->mem_read(reg_pc + 1);
            reg_x = (uint8_t) memory->mem_read(operand + reg_y);
            if(reg_x & 0x80 == 0)
		set_negative(false);
    	    else
		set_negative(true);
    	    if(reg_x == 0x0)
    		set_zero(true);
    	    else 
		set_zero(false);
            break;
        }
	case LDX_A: {
            uint16_t operand = memory->mem_read2(reg_pc + 1);
            reg_x = (uint8_t) memory->mem_read(operand);
            if(reg_x & 0x80 == 0)
		set_negative(false);
    	    else
		set_negative(true);
    	    if(reg_x == 0x0)
    		set_zero(true);
    	    else 
		set_zero(false);
	    break;
	}
	case LDX_AY: {
            uint16_t operand = memory->mem_read2(reg_pc + 1);
            reg_x = (uint8_t) memory->mem_read(operand + reg_y);
            if(reg_x & 0x80 == 0)
		set_negative(false);
    	    else
		set_negative(true);
    	    if(reg_x == 0x0)
    		set_zero(true);
    	    else 
		set_zero(false);
            break;
        }
        case LDY_I: {
            uint16_t operand = memory->mem_read(reg_pc + 1);
	    reg_y = (uint8_t) operand;
            if(reg_y & 0x80 == 0)
		set_negative(false);
    	    else
		set_negative(true);
    	    if(reg_y == 0x0)
    		set_zero(true);
    	    else 
		set_zero(false);
            break;
        }
	case LDY_Z: {
            uint16_t operand = memory->mem_read(reg_pc + 1);
            reg_y = (uint8_t) memory->mem_read(operand);
            if(reg_y & 0x80 == 0)
		set_negative(false);
    	    else
		set_negative(true);
    	    if(reg_y == 0x0)
    		set_zero(true);
    	    else 
		set_zero(false);
            break;
        }
	case LDY_ZX: {
            uint16_t operand = memory->mem_read(reg_pc + 1);
            reg_y = (uint8_t) memory->mem_read(operand + reg_x);
            if(reg_y & 0x80 == 0)
		set_negative(false);
    	    else
		set_negative(true);
    	    if(reg_y == 0x0)
    		set_zero(true);
    	    else 
		set_zero(false);
            break;
        }
	case LDY_A: {
            uint16_t operand = memory->mem_read2(reg_pc + 1);
            reg_y = (uint8_t) memory->mem_read(operand);
            if(reg_y & 0x80 == 0)
		set_negative(false);
    	    else
		set_negative(true);
    	    if(reg_y == 0x0)
    		set_zero(true);
    	    else 
		set_zero(false);
	    break;
	}
	case LDY_AX: {
            uint16_t operand = memory->mem_read2(reg_pc + 1);
            reg_y = (uint8_t) memory->mem_read(operand + reg_x);
            if(reg_y & 0x80 == 0)
		set_negative(false);
    	    else
		set_negative(true);
    	    if(reg_y == 0x0)
    		set_zero(true);
    	    else 
		set_zero(false);
            break;
        }
	case STA_Z: {
            uint16_t operand = memory->mem_read(reg_pc + 1);
            memory->mem_write(operand, reg_ac);
	    break;
	}
	case STA_ZX: {
            uint16_t operand = memory->mem_read(reg_pc + 1);
            memory->mem_write(operand+reg_x, reg_ac);
	    break;
	}
	case STA_A: {
            uint16_t operand = memory->mem_read2(reg_pc + 1);
            memory->mem_write(operand, reg_ac);
	    break;
	}
	case STA_AX: {
            uint16_t operand = memory->mem_read2(reg_pc + 1);
            memory->mem_write(operand+reg_x, reg_ac);
	    break;
	}
	case STA_AY: {
            uint16_t operand = memory->mem_read2(reg_pc + 1);
            memory->mem_write(operand+reg_y, reg_ac);
	    break;
	}
	case STA_IX: {
            uint16_t operand = memory->mem_read(reg_pc + 1);
            memory->mem_write(memory->mem_read2(operand+reg_x), reg_ac);
	    break;
	}
	case STA_IY: {
            uint16_t operand = memory->mem_read(reg_pc + 1);
            memory->mem_write(memory->mem_read2(operand) + reg_y, reg_ac);
	    break;
	}
	case STX_Z: {
            uint16_t operand = memory->mem_read(reg_pc + 1);
            memory->mem_write(operand, reg_x);
	    break;
	}
	case STX_ZY: {
            uint16_t operand = memory->mem_read(reg_pc + 1);
            memory->mem_write(operand+reg_y, reg_x);
	    break;
	}
	case STX_A: {
            uint16_t operand = memory->mem_read2(reg_pc + 1);
            memory->mem_write(operand, reg_x);
	    break;
	}
	case STY_Z: {
            uint16_t operand = memory->mem_read(reg_pc + 1);
            memory->mem_write(operand, reg_y);
	    break;
	}
	case STY_ZX: {
            uint16_t operand = memory->mem_read(reg_pc + 1);
            memory->mem_write(operand+reg_x, reg_y);
	    break;
	}
	case STY_A: {
            uint16_t operand = memory->mem_read2(reg_pc + 1);
            memory->mem_write(operand, reg_y);
	    break;
	}
	case TAX: {
            reg_x = reg_ac;
            if(reg_x & 0x80 == 0)
		set_negative(false);
    	    else
		set_negative(true);
    	    if(reg_x == 0x0)
    		set_zero(true);
    	    else 
		set_zero(false);
	    break;
	}
	case TAY: {
            reg_y = reg_ac;
            if(reg_y & 0x80 == 0)
		set_negative(false);
    	    else
		set_negative(true);
    	    if(reg_y == 0x0)
    		set_zero(true);
    	    else 
		set_zero(false);
	    break;
	}
	case TSX: {
            reg_x = reg_s;
            if(reg_x & 0x80 == 0)
		set_negative(false);
    	    else
		set_negative(true);
    	    if(reg_x == 0x0)
    		set_zero(true);
    	    else 
		set_zero(false);
	    break;
	case TXA: {
            reg_ac = reg_x;
            if(reg_ac & 0x80 == 0)
		set_negative(false);
    	    else
		set_negative(true);
    	    if(reg_ac == 0x0)
    		set_zero(true);
    	    else 
	}
		set_zero(false);
	    break;
	}
	case TXS: {
            reg_s = reg_x;
	    break;
	}
	case TYA: {
            reg_ac = reg_y;
            if(reg_ac & 0x80 == 0)
		set_negative(false);
    	    else
		set_negative(true);
    	    if(reg_ac == 0x0)
    		set_zero(true);
    	    else 
		set_zero(false);
	    break;
	}
	case INX: {
            reg_x = reg_x + 1;
            if(reg_x & 0x80 == 0)
		set_negative(false);
    	    else
		set_negative(true);
    	    if(reg_x == 0x0)
    		set_zero(true);
    	    else 
		set_zero(false);
	    break;
	}
	case INY: {
            reg_y = reg_y + 1;
            if(reg_y & 0x80 == 0)
		set_negative(false);
    	    else
		set_negative(true);
    	    if(reg_y == 0x0)
    		set_zero(true);
    	    else 
		set_zero(false);
	    break;
	}
	case DEX: {
            reg_x = reg_x - 1;
            if(reg_x & 0x80 == 0)
		set_negative(false);
    	    else
		set_negative(true);
    	    if(reg_x == 0x0)
    		set_zero(true);
    	    else 
		set_zero(false);
	    break;
	}
	case DEY: {
            reg_y = reg_y - 1;
            if(reg_y & 0x80 == 0)
		set_negative(false);
    	    else
		set_negative(true);
    	    if(reg_y == 0x0)
    		set_zero(true);
    	    else 
		set_zero(false);
	    break;
	}
	case DEC_Z: {
            uint16_t operand = memory->mem_read(reg_pc + 1);
	    uint8_t result = (uint8_t) memory->mem_read(operand) - 1;
            memory->write(operand, result);
            if(result & 0x80 == 0)
		set_negative(false);
    	    else
		set_negative(true);
    	    if(result == 0x0)
    		set_zero(true);
    	    else 
		set_zero(false);
            break;
        }
	case DEC_ZX: {
            uint16_t operand = memory->mem_read(reg_pc + 1);
	    uint8_t result = (uint8_t) memory->mem_read(operand + reg_x) - 1;
            memory->write(operand + reg_x, result);
            if(result & 0x80 == 0)
		set_negative(false);
    	    else
		set_negative(true);
    	    if(result == 0x0)
    		set_zero(true);
    	    else 
		set_zero(false);
            break;
        }
	case DEC_A: {
            uint16_t operand = memory->mem_read2(reg_pc + 1);
	    uint8_t result = (uint8_t) memory->mem_read(operand) - 1;
            memory->write(operand, result);
            if(result & 0x80 == 0)
		set_negative(false);
    	    else
		set_negative(true);
    	    if(result == 0x0)
    		set_zero(true);
    	    else 
		set_zero(false);
            break;
	}
	case DEC_AX: {
            uint16_t operand = memory->mem_read2(reg_pc + 1);
	    uint8_t result = (uint8_t) memory->mem_read(operand + reg_x) - 1;
            memory->write(operand + reg_x, result);
            if(result & 0x80 == 0)
		set_negative(false);
    	    else
		set_negative(true);
    	    if(result == 0x0)
    		set_zero(true);
    	    else 
		set_zero(false);
            break;
        }
	case INC_Z: {
            uint16_t operand = memory->mem_read(reg_pc + 1);
	    uint8_t result = (uint8_t) memory->mem_read(operand) + 1;
            memory->write(operand, result);
            if(result & 0x80 == 0)
		set_negative(false);
    	    else
		set_negative(true);
    	    if(result == 0x0)
    		set_zero(true);
    	    else 
		set_zero(false);
            break;
        }
	case INC_ZX: {
            uint16_t operand = memory->mem_read(reg_pc + 1);
	    uint8_t result = (uint8_t) memory->mem_read(operand + reg_x) + 1;
            memory->write(operand + reg_x, result);
            if(result & 0x80 == 0)
		set_negative(false);
    	    else
		set_negative(true);
    	    if(result == 0x0)
    		set_zero(true);
    	    else 
		set_zero(false);
            break;
        }
	case INC_A: {
            uint16_t operand = memory->mem_read2(reg_pc + 1);
	    uint8_t result = (uint8_t) memory->mem_read(operand) + 1;
            memory->write(operand, result);
            if(result & 0x80 == 0)
		set_negative(false);
    	    else
		set_negative(true);
    	    if(result == 0x0)
    		set_zero(true);
    	    else 
		set_zero(false);
            break;
	}
	case INC_AX: {
            uint16_t operand = memory->mem_read2(reg_pc + 1);
	    uint8_t result = (uint8_t) memory->mem_read(operand + reg_x) + 1;
            memory->write(operand + reg_x, result);
            if(result & 0x80 == 0)
		set_negative(false);
    	    else
		set_negative(true);
    	    if(result == 0x0)
    		set_zero(true);
    	    else 
		set_zero(false);
            break;
        }
	case SBC_I: {
            uint16_t operand = memory->mem_read(reg_pc + 1);
	    uint8_t src = (uint8_t) operand;
	    uint16_t temp = reg_ac - src - (get_carry() ? 0 : 1);
	    set_negative(temp & 0x80 != 0);
	    set_zero(temp & 0xff == 0);
	    set_overflow(((reg_ac^temp)&0x80) && ((reg_ac^src)&0x80));
	    if(get_demical()) {
		if(((reg_ac&0xf)-(get_carry()?0:1)) < (src&0xf))
			temp -= 6;
		if(temp > 0x99)
			temp -= 0x60;
	    }
	    set_carry(temp<0x100);
	    reg_ac = (uint8_t)temp;
            break;
	}
	case SBC_Z: {
            uint16_t operand = memory->mem_read(reg_pc + 1);
	    uint8_t src = (uint8_t) memory->mem_read(operand);
	    uint16_t temp = reg_ac - src - (get_carry() ? 0 : 1);
	    set_negative(temp & 0x80 != 0);
	    set_zero(temp & 0xff == 0);
	    set_overflow(((reg_ac^temp)&0x80) && ((reg_ac^src)&0x80));
	    if(get_demical()) {
		if(((reg_ac&0xf)-(get_carry()?0:1)) < (src&0xf))
			temp -= 6;
		if(temp > 0x99)
			temp -= 0x60;
	    }
	    set_carry(temp<0x100);
	    reg_ac = (uint8_t)temp;
            break;
	}
	case SBC_ZX: {
            uint16_t operand = memory->mem_read(reg_pc + 1);
	    uint8_t src = (uint8_t) memory->mem_read(operand + reg_x);
	    uint16_t temp = reg_ac - src - (get_carry() ? 0 : 1);
	    set_negative(temp & 0x80 != 0);
	    set_zero(temp & 0xff == 0);
	    set_overflow(((reg_ac^temp)&0x80) && ((reg_ac^src)&0x80));
	    if(get_demical()) {
		if(((reg_ac&0xf)-(get_carry()?0:1)) < (src&0xf))
			temp -= 6;
		if(temp > 0x99)
			temp -= 0x60;
	    }
	    set_carry(temp<0x100);
	    reg_ac = (uint8_t)temp;
            break;
	}
	case SBC_A: {
            uint16_t operand = memory->mem_read2(reg_pc + 1);
	    uint8_t src = (uint8_t) memory->mem_read(operand);
	    uint16_t temp = reg_ac - src - (get_carry() ? 0 : 1);
	    set_negative(temp & 0x80 != 0);
	    set_zero(temp & 0xff == 0);
	    set_overflow(((reg_ac^temp)&0x80) && ((reg_ac^src)&0x80));
	    if(get_demical()) {
		if(((reg_ac&0xf)-(get_carry()?0:1)) < (src&0xf))
			temp -= 6;
		if(temp > 0x99)
			temp -= 0x60;
	    }
	    set_carry(temp<0x100);
	    reg_ac = (uint8_t)temp;
            break;
	}
	case SBC_AX: {
            uint16_t operand = memory->mem_read2(reg_pc + 1);
	    uint8_t src = (uint8_t) memory->mem_read(operand + reg_x);
	    uint16_t temp = reg_ac - src - (get_carry() ? 0 : 1);
	    set_negative(temp & 0x80 != 0);
	    set_zero(temp & 0xff == 0);
	    set_overflow(((reg_ac^temp)&0x80) && ((reg_ac^src)&0x80));
	    if(get_demical()) {
		if(((reg_ac&0xf)-(get_carry()?0:1)) < (src&0xf))
			temp -= 6;
		if(temp > 0x99)
			temp -= 0x60;
	    }
	    set_carry(temp<0x100);
	    reg_ac = (uint8_t)temp;
            break;
	}
	case SBC_AY: {
            uint16_t operand = memory->mem_read2(reg_pc + 1);
	    uint8_t src = (uint8_t) memory->mem_read(operand + reg_y);
	    uint16_t temp = reg_ac - src - (get_carry() ? 0 : 1);
	    set_negative(temp & 0x80 != 0);
	    set_zero(temp & 0xff == 0);
	    set_overflow(((reg_ac^temp)&0x80) && ((reg_ac^src)&0x80));
	    if(get_demical()) {
		if(((reg_ac&0xf)-(get_carry()?0:1)) < (src&0xf))
			temp -= 6;
		if(temp > 0x99)
			temp -= 0x60;
	    }
	    set_carry(temp<0x100);
	    reg_ac = (uint8_t)temp;
            break;
	}
	case SBC_IX: {
            uint16_t operand = memory->mem_read(reg_pc + 1);
	    uint8_t src = (uint8_t) memory->mem_read(memory->mem_read2(operand + reg_x));
	    uint16_t temp = reg_ac - src - (get_carry() ? 0 : 1);
	    set_negative(temp & 0x80 != 0);
	    set_zero(temp & 0xff == 0);
	    set_overflow(((reg_ac^temp)&0x80) && ((reg_ac^src)&0x80));
	    if(get_demical()) {
		if(((reg_ac&0xf)-(get_carry()?0:1)) < (src&0xf))
			temp -= 6;
		if(temp > 0x99)
			temp -= 0x60;
	    }
	    set_carry(temp<0x100);
	    reg_ac = (uint8_t)temp;
            break;
	}
	case SBC_IY: {
            uint16_t operand = memory->mem_read(reg_pc + 1);
	    uint8_t src = (uint8_t) memory->mem_read(memory->mem_read2(operand) + reg_y));
	    uint16_t temp = reg_ac - src - (get_carry() ? 0 : 1);
	    set_negative(temp & 0x80 != 0);
	    set_zero(temp & 0xff == 0);
	    set_overflow(((reg_ac^temp)&0x80) && ((reg_ac^src)&0x80));
	    if(get_demical()) {
		if(((reg_ac&0xf)-(get_carry()?0:1)) < (src&0xf))
			temp -= 6;
		if(temp > 0x99)
			temp -= 0x60;
	    }
	    set_carry(temp<0x100);
	    reg_ac = (uint8_t)temp;
            break;
	}
	case ADC_I: {
            uint16_t operand = memory->mem_read(reg_pc + 1);
	    uint8_t src = (uint8_t) operand;
	    uint16_t temp = reg_ac + src + (get_carry() ? 1 : 0);
	    set_zero(temp & 0xff == 0);
	    if(get_demical()) {
		if(((reg_ac&0xf) + (src&0xf) + (get_carry()?1:0)) > 9)
			temp += 6;
	    	set_negative(temp & 0x80 != 0);
	    	set_overflow(((reg_ac^temp)&0x80) && !((reg_ac^src)&0x80));
	    	if(temp > 0x99)
			temp += 0x60;
		set_carry(temp > 0x99);
	    }else{
	    	set_negative(temp & 0x80 != 0);
	    	set_overflow(((reg_ac^temp)&0x80) && !((reg_ac^src)&0x80));
	    	set_carry(temp > 0xff);
	    }
	    reg_ac = (uint8_t) temp;
            break;
	}
	case ADC_Z: {
            uint16_t operand = memory->mem_read(reg_pc + 1);
	    uint8_t src = (uint8_t) memory->mem_read(operand);
	    uint16_t temp = reg_ac + src + (get_carry() ? 1 : 0);
	    set_zero(temp & 0xff == 0);
	    if(get_demical()) {
		if(((reg_ac&0xf) + (src&0xf) + (get_carry()?1:0)) > 9)
			temp += 6;
	    	set_negative(temp & 0x80 != 0);
	    	set_overflow(((reg_ac^temp)&0x80) && !((reg_ac^src)&0x80));
	    	if(temp > 0x99)
			temp += 0x60;
		set_carry(temp > 0x99);
	    }else{
	    	set_negative(temp & 0x80 != 0);
	    	set_overflow(((reg_ac^temp)&0x80) && !((reg_ac^src)&0x80));
	    	set_carry(temp > 0xff);
	    }
	    reg_ac = (uint8_t) temp;
            break;
	}
	case ADC_ZX: {
            uint16_t operand = memory->mem_read(reg_pc + 1);
	    uint8_t src = (uint8_t) memory->mem_read(operand + reg_x);
	    uint16_t temp = reg_ac + src + (get_carry() ? 1 : 0);
	    set_zero(temp & 0xff == 0);
	    if(get_demical()) {
		if(((reg_ac&0xf) + (src&0xf) + (get_carry()?1:0)) > 9)
			temp += 6;
	    	set_negative(temp & 0x80 != 0);
	    	set_overflow(((reg_ac^temp)&0x80) && !((reg_ac^src)&0x80));
	    	if(temp > 0x99)
			temp += 0x60;
		set_carry(temp > 0x99);
	    }else{
	    	set_negative(temp & 0x80 != 0);
	    	set_overflow(((reg_ac^temp)&0x80) && !((reg_ac^src)&0x80));
	    	set_carry(temp > 0xff);
	    }
	    reg_ac = (uint8_t) temp;
            break;
	}
	case ADC_A: {
            uint16_t operand = memory->mem_read2(reg_pc + 1);
	    uint8_t src = (uint8_t) memory->mem_read(operand);
	    uint16_t temp = reg_ac + src + (get_carry() ? 1 : 0);
	    set_zero(temp & 0xff == 0);
	    if(get_demical()) {
		if(((reg_ac&0xf) + (src&0xf) + (get_carry()?1:0)) > 9)
			temp += 6;
	    	set_negative(temp & 0x80 != 0);
	    	set_overflow(((reg_ac^temp)&0x80) && !((reg_ac^src)&0x80));
	    	if(temp > 0x99)
			temp += 0x60;
		set_carry(temp > 0x99);
	    }else{
	    	set_negative(temp & 0x80 != 0);
	    	set_overflow(((reg_ac^temp)&0x80) && !((reg_ac^src)&0x80));
	    	set_carry(temp > 0xff);
	    }
	    reg_ac = (uint8_t) temp;
            break;
	}
	case ADC_AX: {
            uint16_t operand = memory->mem_read2(reg_pc + 1);
	    uint8_t src = (uint8_t) memory->mem_read(operand + reg_x);
	    uint16_t temp = reg_ac + src + (get_carry() ? 1 : 0);
	    set_zero(temp & 0xff == 0);
	    if(get_demical()) {
		if(((reg_ac&0xf) + (src&0xf) + (get_carry()?1:0)) > 9)
			temp += 6;
	    	set_negative(temp & 0x80 != 0);
	    	set_overflow(((reg_ac^temp)&0x80) && !((reg_ac^src)&0x80));
	    	if(temp > 0x99)
			temp += 0x60;
		set_carry(temp > 0x99);
	    }else{
	    	set_negative(temp & 0x80 != 0);
	    	set_overflow(((reg_ac^temp)&0x80) && !((reg_ac^src)&0x80));
	    	set_carry(temp > 0xff);
	    }
	    reg_ac = (uint8_t) temp;
            break;
	}
	case ADC_AY: {
            uint16_t operand = memory->mem_read2(reg_pc + 1);
	    uint8_t src = (uint8_t) memory->mem_read(operand + reg_y);
	    uint16_t temp = reg_ac + src + (get_carry() ? 1 : 0);
	    set_zero(temp & 0xff == 0);
	    if(get_demical()) {
		if(((reg_ac&0xf) + (src&0xf) + (get_carry()?1:0)) > 9)
			temp += 6;
	    	set_negative(temp & 0x80 != 0);
	    	set_overflow(((reg_ac^temp)&0x80) && !((reg_ac^src)&0x80));
	    	if(temp > 0x99)
			temp += 0x60;
		set_carry(temp > 0x99);
	    }else{
	    	set_negative(temp & 0x80 != 0);
	    	set_overflow(((reg_ac^temp)&0x80) && !((reg_ac^src)&0x80));
	    	set_carry(temp > 0xff);
	    }
	    reg_ac = (uint8_t) temp;
            break;
	}
	case ADC_IX: {
            uint16_t operand = memory->mem_read(reg_pc + 1);
	    uint8_t src = (uint8_t) memory->mem_read(memory->mem_read2(operand + reg_x));
	    uint16_t temp = reg_ac + src + (get_carry() ? 1 : 0);
	    set_zero(temp & 0xff == 0);
	    if(get_demical()) {
		if(((reg_ac&0xf) + (src&0xf) + (get_carry()?1:0)) > 9)
			temp += 6;
	    	set_negative(temp & 0x80 != 0);
	    	set_overflow(((reg_ac^temp)&0x80) && !((reg_ac^src)&0x80));
	    	if(temp > 0x99)
			temp += 0x60;
		set_carry(temp > 0x99);
	    }else{
	    	set_negative(temp & 0x80 != 0);
	    	set_overflow(((reg_ac^temp)&0x80) && !((reg_ac^src)&0x80));
	    	set_carry(temp > 0xff);
	    }
	    reg_ac = (uint8_t) temp;
            break;
	}
	case ADC_IY: {
            uint16_t operand = memory->mem_read(reg_pc + 1);
	    uint8_t src = (uint8_t) memory->mem_read(memory->mem_read2(operand) + reg_y));
	    uint16_t temp = reg_ac + src + (get_carry() ? 1 : 0);
	    set_zero(temp & 0xff == 0);
	    if(get_demical()) {
		if(((reg_ac&0xf) + (src&0xf) + (get_carry()?1:0)) > 9)
			temp += 6;
	    	set_negative(temp & 0x80 != 0);
	    	set_overflow(((reg_ac^temp)&0x80) && !((reg_ac^src)&0x80));
	    	if(temp > 0x99)
			temp += 0x60;
		set_carry(temp > 0x99);
	    }else{
	    	set_negative(temp & 0x80 != 0);
	    	set_overflow(((reg_ac^temp)&0x80) && !((reg_ac^src)&0x80));
	    	set_carry(temp > 0xff);
	    }
	    reg_ac = (uint8_t) temp;
            break;
	}
        case AND_I: {
            uint16_t operand = memory->mem_read(reg_pc + 1);
	    reg_ac &= (uint8_t) operand;
            if(reg_ac & 0x80 == 0)
		set_negative(false);
    	    else
		set_negative(true);
    	    if(reg_ac == 0x0)
    		set_zero(true);
    	    else 
		set_zero(false);
            break;
        }
	case AND_Z: {
            uint16_t operand = memory->mem_read(reg_pc + 1);
            reg_ac &= (uint8_t) memory->mem_read(operand);
            if(reg_ac & 0x80 == 0)
		set_negative(false);
    	    else
		set_negative(true);
    	    if(reg_ac == 0x0)
    		set_zero(true);
    	    else 
		set_zero(false);
            break;
        }
	case AND_ZX: {
            uint16_t operand = memory->mem_read(reg_pc + 1);
            reg_ac &= (uint8_t) memory->mem_read(operand + reg_x);
            if(reg_ac & 0x80 == 0)
		set_negative(false);
    	    else
		set_negative(true);
    	    if(reg_ac == 0x0)
    		set_zero(true);
    	    else 
		set_zero(false);
            break;
        }
	case AND_A: {
            uint16_t operand = memory->mem_read2(reg_pc + 1);
            reg_ac &= (uint8_t) memory->mem_read(operand);
            if(reg_ac & 0x80 == 0)
		set_negative(false);
    	    else
		set_negative(true);
    	    if(reg_ac == 0x0)
    		set_zero(true);
    	    else 
		set_zero(false);
	    break;
	}
	case AND_AX: {
            uint16_t operand = memory->mem_read2(reg_pc + 1);
            reg_ac &= (uint8_t) memory->mem_read(operand + reg_x);
            if(reg_ac & 0x80 == 0)
		set_negative(false);
    	    else
		set_negative(true);
    	    if(reg_ac == 0x0)
    		set_zero(true);
    	    else 
		set_zero(false);
            break;
        }
	case AND_AY: {
            uint16_t operand = memory->mem_read2(reg_pc + 1);
            reg_ac &= (uint8_t) memory->mem_read(operand + reg_y);
            if(reg_ac & 0x80 == 0)
		set_negative(false);
    	    else
		set_negative(true);
    	    if(reg_ac == 0x0)
    		set_zero(true);
    	    else 
		set_zero(false);
            break;
        }
	case AND_IX: {
            uint16_t operand = memory->mem_read(reg_pc + 1);
            reg_ac &= (uint8_t) memory->mem_read(memory->mem_read2(operand + reg_x));
            if(reg_ac & 0x80 == 0)
		set_negative(false);
    	    else
		set_negative(true);
    	    if(reg_ac == 0x0)
    		set_zero(true);
    	    else 
		set_zero(false);
            break;
        }
	case AND_IY: {
            uint16_t operand = memory->mem_read(reg_pc + 1);
            reg_ac &= (uint8_t) memory->mem_read((memory->mem_read2(operand) + reg_y);
            if(reg_ac & 0x80 == 0)
		set_negative(false);
    	    else
		set_negative(true);
    	    if(reg_ac == 0x0)
    		set_zero(true);
    	    else 
		set_zero(false);
            break;
        }
    }
}

void CPU::set_negative(bool value) {
    reg_p &= 0x7f;
    reg_p |= (((uint8_t) value) << 7);
}

void CPU::set_overflow(bool value) {
    reg_p &= 0xbf;
    reg_p |= (((uint8_t) value) << 6);
}

void CPU::set_break(bool value) {
    reg_p &= 0xef;
    reg_p |= (((uint8_t) value) << 4);
}

void CPU::set_decimal(bool value) {
    reg_p &= 0xf7;
    reg_p |= (((uint8_t) value) << 3);
}

void CPU::set_interrupt(bool value) {
    reg_p &= 0xfb;
    reg_p |= (((uint8_t) value) << 2);
}

void CPU::set_zero(bool value) {
    reg_p &= 0xfd;
    reg_p |= (((uint8_t) value) << 1);
}

void CPU::set_carry(bool value) {
    reg_p &= 0xfe;
    reg_p |= value;
}

bool CPU::get_negative() {
    return (bool) (reg_p >> 7);
}

bool CPU::get_overflow() {
    return (bool) ((reg_p & 0x40) >> 6);
}

bool CPU::get_break() {
    return (bool) ((reg_p & 0x10) >> 4);
}

bool CPU::get_decimal() {
    return (bool) ((reg_p & 0x8) >> 3);
}

bool CPU::get_interrupt() {
    return (bool) ((reg_p & 0x4) >> 2);
}

bool CPU::get_zero() {
    return (bool) ((reg_p & 0x2) >> 1);
}

bool CPU::get_carry() {
    return (bool) (reg_p & 0x1);
}

uint8_t CPU::get_ac() {
    return reg_ac;
}

uint8_t CPU::get_x() {
    return reg_x;
}

uint8_t CPU::get_y() {
    return reg_y;
}

uint16_t CPU::get_pc() {
    return reg_pc;
}

uint8_t CPU:: get_s() {
    return reg_s;
}


