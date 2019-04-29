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
            break;
        }
        case ADC_I: {
            // do something
            break;
        }
        case SEI: {
            set_interrupt(1);
            break;
        }

        case BCC: {
	    uint16_t operand = memory->mem_read(reg_pc + 1);
            if (!get_carry()){
		reg_pc += operand;
	    }

	    reg_pc += 2;
	    break;
	}

        case BCS: {
	    uint16_t operand = memory->mem_read(reg_pc + 1);
	    if (get_carry()){
		reg_pc += operand;
	    }

	    reg_pc += 2;
	    break;

	}

	case BEQ: {
	    uint16_t operand = memory->mem_read(reg_pc + 1);
	    if (get_zero()) {
		reg_pc += operand;
	    }	
	    reg_pc += 2;
	    break;
	}

        case BNE: {
	    uint16_t operand = memory->mem_read(reg_pc + 1);
	    if (!get_zero()) {
		reg_pc += operand;
	    }

	    reg_pc += 2;
	    break;

	}

        case BVC: {
	    uint16_t operand = memory->mem_read(reg_pc + 1);
	    if (!get_overflow()) {
		reg_pc += operand;
	    }
	    reg_pc += 2;
	    break;
	}

        case BVS: {
	    uint16_t operand = memory->mem_read(reg_pc + 1);
	    if (get_overflow()) {
		reg_pc += operand;
	    }

	    reg_pc += 2;
	    break;
        }

        case BPL: {
	    uint16_t operand = memory->mem_read(reg_pc + 1);
	    if (!get_negative()) {
		reg_pc += operand;
	    }
	    
	    reg_pc += 2;
	    break;
	}

        case BMI: {
	    uint16_t operand = memory->mem_read(reg_pc + 1);
	    if (get_negative()) {
		reg_pc += operand;
	    }

	    reg_pc += 2;
	    break;
	}

        case JMP_I: {
            uint16_t operand = memory->mem_read(reg_pc + 1) + (memory->mem_read(reg_pc + 2) << 8);
	    uint16_t nextMem = ((operand + 1) % 0xFF) + (operand >> 8);

	    reg_pc = memory->mem_read(operand) + (memory->mem_read(nextMem) << 8);
	
	    break;
	}

	case JMP_A: {
	    reg_pc = memory->mem_read(reg_pc + 1) + (memory->mem_read(reg_pc + 2) << 8);
	    break;
	}
	
        case JSR: {
	    uint16_t operand = memory->mem_read(reg_pc + 1) + (memory->mem_read(reg_pc + 2) << 8) - 1;
	    memory->mem_write(reg_s - 1, operand >> 8);
	    memory->mem_write(reg_s - 2, operand % 0xFF);
	    reg_s -= 2;
	    reg_pc = operand + 1;	    
  
	    break;
	}

        case RTI: {
	    reg_p = memory->mem_read(reg_s) + (memory->mem_read(reg_s - 1) << 8);
	    reg_pc = memory->mem_read(reg_s - 2) + (memory->mem_read(reg_s - 3) << 8);
	    reg_s += 4;    

	    break;
	}

	case RTS: {
            uint16_t operand = memory->mem_read(reg_s) + (memory->mem_read(reg_s + 1) << 8);
	    reg_s += 2;
	    reg_pc = operand + 1;

	    break;
	}

	
        
        // add all the other opcodes
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
