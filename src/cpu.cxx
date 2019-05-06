#include "cpu.hpp"

CPU::CPU(std::shared_ptr<Mem> memory) {
    this->memory = memory;
    total_cycles = 7;
    reg_s = 0xFD;
    reg_p = 0x24;
    reg_pc = 0xC000;
    //::cout << std::hex << unsigned(reg_pc) << std::endl;
}

uint16_t CPU::execute() {
    cycles = 0;
    std::cout << std::hex << unsigned(reg_pc) << " ";
    std::cout << "A: " << std::setw(2) << std::hex << unsigned(reg_ac) << " ";
    std::cout << "X: " << std::setw(2) << std::hex << unsigned(reg_x) << " ";
    std::cout << "Y: " << std::setw(2) << std::hex << unsigned(reg_y) << " ";
    std::cout << "P: " << std::setw(2) << std::hex << unsigned(reg_p) << " ";
    std::cout << "SP: " << std::setw(2) << std::hex << unsigned(reg_s) << " ";
    uint8_t opcode = pc_read();
    
    switch (opcode) {
        case LDA_I: {
            lda(imm());
            break;
        }
        case LDA_Z: {
            lda(zp());
            break;
        }
        case LDA_ZX: {
            lda(zp_x());
            break;
        }
        case LDA_A: {
            lda(abs());
            break;
        } 
        case LDA_AX: {
            lda(abs_x());
            break;
        }
        case LDA_AY: {
            lda(abs_y());
            break;
        }
        case LDA_IX: {
            lda(ind_x());
            break;
        }
        case LDA_IY: {
            lda(ind_y());
            break;
        }
        case LDX_I: {
            ldx(imm());
            break;
        }
        case LDX_Z: {
            ldx(zp());
            break;
        }
        case LDX_ZY: {
            ldx(zp_y());
            break;
        }
        case LDX_A: {   
            ldx(abs());
            break;
        }
        case LDX_AY: {
            ldx(abs_y());
            break;
        }
        case LDY_I: {
            ldy(imm());
            break;
        }
        case LDY_Z: {
            ldy(zp());
            break;
        }
        case LDY_ZX: {
            ldy(zp_x());
            break;
        }
        case LDY_A: {
            ldy(abs());
            break;
        }
        case LDY_AX: {
            ldy(abs_x());
            break;
        }
        case STA_Z: {
            uint8_t address = pc_read();
            mem_write(address, reg_ac);
            break;
        }
        case STA_ZX: {
            uint8_t address = pc_read();
            mem_write(address + reg_x, reg_ac);
            cycles++;
            break;
        }
        case STA_A: {
            uint16_t address = pc_read2();
            mem_write(address, reg_ac);
            break;
        }
        case STA_AX: {
            uint16_t address = pc_read2();
            uint16_t shift = address + reg_x;
            PAGE_SHIFT(shift, address);
            mem_write(shift, reg_ac);
            break;
        }
        case STA_AY: {
            uint16_t address = pc_read2();
            uint16_t shift = address + reg_y;
            PAGE_SHIFT(shift, address);
            mem_write(shift, reg_ac);
            break;
        }
        // check over everything after this
        case STA_IX: {
            uint8_t operand = pc_read();
            uint16_t address = mem_read2((uint8_t) (operand + reg_x));
            cycles++;
            mem_write(address, reg_ac);
            break;
        }
        case STA_IY: {
            uint8_t operand = pc_read();
            uint16_t address = mem_read2(operand);
            uint16_t shift = address + reg_y;
            PAGE_SHIFT(shift, address);
            
            mem_write(shift, reg_ac);
            break;
        }
        // here
        case STX_Z: {
            uint16_t operand =pc_read();
            mem_write(operand, reg_x);
            break;
        }
        case STX_ZY: {
            uint16_t operand = pc_read();
            mem_write(operand+reg_y, reg_x);
            break;
        }
        case STX_A: {
            uint16_t operand = pc_read2();
            mem_write(operand, reg_x);
            break;
        }
        case STY_Z: {
            uint16_t operand = pc_read();
            mem_write(operand, reg_y);
            break;
        }
        case STY_ZX: {
            uint16_t operand = pc_read();
            mem_write(operand+reg_x, reg_y);
            break;
        }
        case STY_A: {
            uint16_t operand = pc_read2();
            mem_write(operand, reg_y);
            break;
        }
        case TAX: {
            reg_x = reg_ac;
            check_nz(reg_x);
            break;
        }
        case TAY: {
            reg_y = reg_ac;
            check_nz(reg_y);
            break;
        }
        case TSX: {
            reg_x = reg_s;
            check_nz(reg_x);
            break;
        }
        case TXA: {
            reg_ac = reg_x;
            check_nz(reg_ac);
            break;
        }
        case TXS: {
            reg_s = reg_x;
            break;
        }
        case TYA: {
            reg_ac = reg_y;
            check_nz(reg_ac);
            break;
        }
        case INX: {
            reg_x = reg_x + 1;
            cycles++;
            check_nz(reg_x);
            break;
        }
        case INY: {
            reg_y = reg_y + 1;
            cycles++;
            check_nz(reg_y);
            break;
        }
        case DEX: {
            reg_x = reg_x - 1;
            cycles++;
            check_nz(reg_x);
            break;
        }
        case DEY: {
            reg_y = reg_y - 1;
            cycles++;
            check_nz(reg_y);
            break;
        }
        case DEC_Z: {
            uint8_t operand = pc_read();
            uint8_t result = mem_read(operand) - 1;
            mem_write(operand, result);
            cycles++;
            check_nz(result);
            break;
        }
        case DEC_ZX: {
            uint8_t operand = pc_read();
            uint8_t address = operand + reg_x;
            uint8_t result = mem_read(address) - 1;
            mem_write(address, result);
            cycles += 2;
            check_nz(result);
            break;
        }
        case DEC_A: {
            uint16_t address = pc_read2();
            uint8_t result = mem_read(address) - 1;
            mem_write(address, result);
            cycles++;
            check_nz(result);
            break;
        }
        case DEC_AX: {
            uint16_t operand = pc_read2();
            uint16_t address = operand + reg_x;
            uint8_t result = mem_read(address) - 1;
            mem_write(address, result);
            cycles += 2;
            check_nz(result);
            break;
        }
        case INC_Z: {
            uint8_t operand = pc_read();
            uint8_t result = (uint8_t) mem_read(operand) + 1;
            mem_write(operand, result);
            cycles++;
            check_nz(result);
            break;
        }
        case INC_ZX: {
            uint8_t operand = pc_read();
            uint8_t address = operand + reg_x;
            uint8_t result = mem_read(address) + 1;
            mem_write(address, result);
            cycles += 2;
            check_nz(result);
            break;
        }
        case INC_A: {
            uint16_t address = pc_read2();
            uint8_t result = mem_read(address) + 1;
            mem_write(address, result);
            cycles++;
            check_nz(result);
            break;
        }
        case INC_AX: {
            uint16_t operand = pc_read2();
            uint16_t address = operand + reg_x;
            uint8_t result = mem_read(address) + 1;
            mem_write(address, result);
            cycles += 2;
            check_nz(result);
            break;
        }
        case SBC_I: {
            sbc(imm());
            break;
        }
        case SBC_Z: {
            sbc(zp());
            break;
        }
        case SBC_ZX: {
            sbc(zp_x());
            break;
        }
        case SBC_A: {
            sbc(abs());
            break;
        }
        case SBC_AX: {
            sbc(abs_x());
            break;
        }
        case SBC_AY: {
            sbc(abs_y());
            break;
        }
        case SBC_IX: {
            sbc(ind_x());
            break;
        }
        case SBC_IY: {
            sbc(ind_y());
            break;
        }
        case ADC_I: {
            adc(imm());
            break;
        }
        case ADC_Z: {
            adc(zp());
            break;
        }
        case ADC_ZX: {
            adc(zp_x());
            break;
        }
        case ADC_A: {
            adc(abs());
            break;
        }
        case ADC_AX: {
            adc(abs_x());
            break;
        }
        case ADC_AY: {
            adc(abs_y());
            break;
        }
        case ADC_IX: {
            adc(ind_x());
            break;
        }
        case ADC_IY: {
            adc(ind_y());
            break;
        }
        /////////
        // AND //
        /////////

        //A logical AND is performed, bit by bit, on the accumulator contents 
        // using the contents of a byte of memory.

        case AND_I: {
            uint16_t operand = pc_read();
            reg_ac &= (uint8_t) operand;
            check_nz(reg_ac);
            break;
        }
        case AND_Z: {
            uint8_t operand = pc_read();
            reg_ac &= (uint8_t) mem_read(operand);
            check_nz(reg_ac);
            break;
        }
        case AND_ZX: {
            uint8_t operand = pc_read();
            reg_ac &= mem_read(operand + reg_x);
            check_nz(reg_ac);
            break;
        }
        case AND_A: {
            uint16_t operand = pc_read2();
            reg_ac &= mem_read(operand);
            check_nz(reg_ac);
            break;
        }
        case AND_AX: {
            uint16_t operand = pc_read2();
            reg_ac &= mem_read(operand + reg_x);
            check_nz(reg_ac);
            break;
        }
        case AND_AY: {
            uint16_t operand = pc_read2();
            reg_ac &= mem_read(operand + reg_y);
            check_nz(reg_ac);
            break;
        }
        case AND_IX: {
            uint16_t operand = pc_read();
            reg_ac &= mem_read(mem_read2(operand + reg_x));
            check_nz(reg_ac);
            break;
        }
        case AND_IY: {
            uint16_t operand = pc_read();
            reg_ac &= mem_read(mem_read2(operand) + reg_y);
            check_nz(reg_ac);
            break;
        }
        /////////
        // ASL //
        /////////

        // This operation shifts all the bits of the accumulator or memory contents one bit left.
        // Bit 0 is set to 0 and bit 7 is placed in the carry flag.
        // The effect of this operation is to multiply the memory contents by 2
        // (ignoring 2's complement considerations), setting the carry if the result will not fit in 8 bits.

        case ASL_AC: {
            set_carry(reg_ac >> 7);
            
            reg_ac = reg_ac << 1;
            set_negative(NEGATIVE(reg_ac));
            set_zero(ZERO(reg_ac));
            break;
        }
        case ASL_Z: {
            uint8_t address = pc_read();
            uint16_t operand = mem_read(address);

            set_carry(operand >> 7);
            operand = operand << 1;
            mem_write(address, operand);
            set_negative(NEGATIVE(operand));
            set_zero(ZERO(operand)); 
            break;
        }
        case ASL_ZX: {
            uint8_t address = pc_read();
            uint16_t operand = mem_read(address + reg_x);
            cycles++;
            set_carry(operand >> 7);
            operand = operand << 1;
            mem_write(address, operand);
            set_negative(NEGATIVE(operand));
            set_zero(ZERO(operand)); 
            break;
        }
        case ASL_A: {
            uint8_t address = pc_read2();
            uint16_t operand = mem_read(address);

            set_carry(operand >> 7);
            //reg_ac = reg_ac << 1;
            operand = operand << 1;
            mem_write(address, operand);
            set_negative(NEGATIVE(operand));
            set_zero(ZERO(operand));
            break;
        }
        case ASL_AX: {
            uint16_t address = pc_read2();
            uint16_t shift = address + reg_x;
            
            // if adding the value of register x crosses a page boundary, take another cycle
            PAGE_SHIFT(shift, address);
            
            uint8_t operand = mem_read(shift);

            set_carry(operand >> 7);
            //reg_ac = reg_ac << 1;
            operand = operand << 1;
            mem_write(address, operand);
            set_negative(NEGATIVE(operand));
            break;
        }

        /////////
        // BIT //
        /////////

        // BIT sets the Z flag as though the value in the address tested were ANDed with the accumulator.
        // The S and V flags are set to match bits 7 and 6 respectively in the value stored at the tested address.
        
        case BIT_Z: {
            
            uint8_t operand = zp();
            set_zero(ZERO((reg_ac & operand)));
            set_negative(operand >> 7);
            set_overflow((operand & 0x40) >> 6);
            break;
        }
        case BIT_A: {
            uint8_t operand = abs();
            set_zero(ZERO((reg_ac & operand)));
            
            set_negative(operand >> 7);
            set_overflow((operand & 0x40) >> 6);
            break;
        }

        /////////
        // EOR //
        /////////

        // EOR (Exclusive OR Memory With Accumulator) performs a logical XOR on the operand 
        // and the accumulator and stores the result in the accumulator. 
        // This opcode is similar in function to AND and ORA.

        case EOR_I: {
            uint16_t operand = reg_ac ^ pc_read();
            set_negative(NEGATIVE(operand));
            set_zero(ZERO(operand));

            reg_ac = (uint8_t) operand;
            break;
        }
        case EOR_Z: {
            uint8_t address = pc_read();
            uint8_t operand = reg_ac ^ mem_read(address);
            
            set_negative(NEGATIVE(operand));
            set_zero(ZERO(operand));

            reg_ac = (uint8_t) operand;
            break;
        }
        case EOR_ZX: {
            uint8_t address = pc_read();
            uint8_t operand = reg_ac ^ mem_read(address + reg_x);
            cycles++;

            set_negative(NEGATIVE(operand));
            set_zero(ZERO(operand));

            reg_ac = (uint8_t) operand;
            break;
        }
        case EOR_A: {
            uint16_t address = pc_read2();
            uint8_t operand = reg_ac ^ mem_read(address);
            
            set_negative(NEGATIVE(operand));
            set_zero(ZERO(operand));

            reg_ac = (uint8_t) operand;
            break;
        }
        case EOR_AX: {
            uint16_t address = pc_read2();
            uint16_t shift = address + reg_x;
            
            // if adding the value of register x crosses a page boundary, take another cycle
            PAGE_SHIFT(shift, address);
            
            uint8_t operand = reg_ac ^ mem_read(shift);
            set_negative(NEGATIVE(operand));
            set_zero(ZERO(operand));
            reg_ac = (uint8_t) operand;
            break;
        }
        case EOR_AY: {
            uint16_t address = pc_read2();
            uint16_t shift = address + reg_y;
            
            // if adding the value of register x crosses a page boundary, take another cycle
            PAGE_SHIFT(shift, address);
            
            uint8_t operand = reg_ac ^ mem_read(shift);
            set_negative(NEGATIVE(operand));
            set_zero(ZERO(operand));

            reg_ac = (uint8_t) operand;
            break;
        }
        case EOR_IX: {
            uint8_t operand = pc_read();
            uint16_t address = mem_read2((uint8_t) (operand + reg_x));
            uint8_t value = reg_ac ^ mem_read(address);
            cycles++;
            set_negative(NEGATIVE(value));
            set_zero(ZERO(operand));
            reg_ac = (uint8_t) value;
            break;
        }
        case EOR_IY: {
            uint8_t operand = pc_read();
            uint16_t address = mem_read2(operand);
            uint16_t shift = address + reg_y;
            
            PAGE_SHIFT(shift, address);
            
            uint8_t value = reg_ac ^ mem_read(address + reg_y);
            set_negative(NEGATIVE(value));
            set_zero(ZERO(value));
            
            reg_ac = (uint8_t) value;
            break;
        }
        
        /////////
        // LSR //
        /////////

        // Each of the bits in A or M is shift one place to the right.
        // The bit that was in bit 0 is shifted into the carry flag. Bit 7 is set to zero.

        case LSR_AC: {
            set_carry((reg_ac << 7) >> 7);
            reg_ac = reg_ac >> 1;
            break;
        }
        case LSR_Z: {
            uint8_t address = pc_read();
            uint8_t operand = mem_read(address);
            set_carry((operand << 7) >> 7);
            operand = operand >> 1;
            mem_write(address, operand);
            break;
        }
        case LSR_ZX: {
            uint8_t address = pc_read();
            uint8_t operand = mem_read(address + reg_x);
            cycles++;
            set_carry((operand << 7) >> 7);
            operand = operand >> 1;
            mem_write(address, operand);
            break;
        }
        case LSR_A: {
            uint8_t address = pc_read2();
            uint8_t operand = mem_read(address);
            set_carry((operand << 7) >> 7);
            operand = operand >> 1;
            mem_write(address, operand);
            break;
        }
        case LSR_AX: {
            uint16_t address = pc_read2();
            uint16_t shift = address + reg_x;
            
            // if adding the value of register x crosses a page boundary, take another cycle
            PAGE_SHIFT(shift, address);
            
            uint8_t operand = mem_read(shift);

            set_carry((operand << 7) >> 7);
            operand = operand >> 1;
            mem_write(address, operand);
            break;
        }

        /////////
        // ORA //
        /////////

        // ORA (Or Memory With Accumulator) performs a logical OR on the operand and the accumulator
        // and stores the result in the accumulator. This opcode is similar in function to AND and EOR.
        
        case ORA_I: {
            uint16_t operand = reg_ac | pc_read();
            set_negative(NEGATIVE(operand));
            set_zero(ZERO(operand));

            reg_ac = (uint8_t) operand;
            break;
        }
        case ORA_Z: {
            uint8_t address = pc_read();
            uint8_t operand = reg_ac | mem_read(address);
            
            set_negative(NEGATIVE(operand));
            set_zero(ZERO(operand));

            reg_ac = (uint8_t) operand;
            break;
        }
        case ORA_ZX: {
            uint8_t address = pc_read();
            uint8_t operand = reg_ac | mem_read(address + reg_x);
            cycles++;

            set_negative(NEGATIVE(operand));
            set_zero(ZERO(operand));

            reg_ac = (uint8_t) operand;
            break;
        }
        case ORA_A: {
            uint16_t address = pc_read2();
            uint8_t operand = reg_ac | mem_read(address);
            
            set_negative(NEGATIVE(operand));
            set_zero(ZERO(operand));

            reg_ac = (uint8_t) operand;
            break;
        }
        case ORA_AX: {
            uint16_t address = pc_read2();
            uint16_t shift = address + reg_x;
            
            // if adding the value of register x crosses a page boundary, take another cycle
            PAGE_SHIFT(shift, address);
            
            uint8_t operand = reg_ac | mem_read(shift);
            set_negative(NEGATIVE(operand));
            set_zero(ZERO(operand));
            reg_ac = (uint8_t) operand;
            break;
        }
        case ORA_AY: {
            uint16_t address = pc_read2();
            uint16_t shift = address + reg_y;
            
            // if adding the value of register x crosses a page boundary, take another cycle
            PAGE_SHIFT(shift, address);
            
            uint8_t operand = reg_ac | mem_read(shift);
            set_negative(NEGATIVE(operand));
            set_zero(ZERO(operand));

            reg_ac = (uint8_t) operand;
            break;
        }
        case ORA_IX: {
            uint8_t operand = pc_read();
            uint16_t address = mem_read2((uint8_t) (operand + reg_x));
            uint8_t value = reg_ac | mem_read(address);
            cycles++;
            set_negative(NEGATIVE(value));
            set_zero(ZERO(operand));
            reg_ac = (uint8_t) value;
            break;
        }
        case ORA_IY: {
            uint8_t operand = pc_read();
            uint16_t address = mem_read2(operand);
            uint16_t shift = address + reg_y;
            
            PAGE_SHIFT(shift, address);
            
            uint8_t value = reg_ac | mem_read(address + reg_y);
            set_negative(NEGATIVE(value));
            set_zero(ZERO(value));
            
            reg_ac = (uint8_t) value;
            break;
        }

        /////////
        // ROL //
        /////////

        // ROL shifts all bits left one position. 
        // The Carry is shifted into bit 0 and the original bit 7 is shifted into the Carry.

        case ROL_AC: {
            uint8_t old_carry = get_carry();
            set_carry(reg_ac >> 7);
            reg_ac = (reg_ac << 1) | old_carry;
            break; 
        }
        // Change based on memory in the future
        case ROL_Z: {
            uint8_t address = pc_read();
            uint8_t operand = mem_read(address);
            uint8_t old_carry = get_carry();
            set_carry(operand >> 7);
            operand = (operand << 1) | old_carry; 
            mem_write(address, operand);
            break;
        }
        case ROL_ZX: {
            uint8_t address = pc_read();
            uint8_t operand = mem_read(address + reg_x);
            uint8_t old_carry = get_carry();
            cycles++;
            set_carry(operand >> 7);
            operand = (operand << 1) | old_carry; 
            mem_write(address, operand);
            break;
        }
        case ROL_A: {
            uint8_t address = pc_read2();
            uint8_t operand = mem_read(address);
            uint8_t old_carry = get_carry();
            set_carry(operand >> 7);
            operand = (operand << 1) | old_carry; 
            mem_write(address, operand);
            break;
        }
        case ROL_AX: {
            uint8_t address = pc_read();
            uint16_t shift = address + reg_x;
            
            // if adding the value of register x crosses a page boundary, take another cycle
            PAGE_SHIFT(shift, address);
            
            uint8_t operand = mem_read(shift);
            uint8_t old_carry = get_carry();
            set_carry(operand >> 7);
            operand = (operand << 1) | old_carry; 
            mem_write(address, operand); 
            break;
        }

        /////////
        // ROR //
        /////////

        // Move each of the bits in either A or M one place to the right. 
        // Bit 7 is filled with the current value of the carry flag whilst the old bit 0 becomes the new carry flag value.

        case ROR_AC: {
            uint8_t old_carry = get_carry();
            set_carry((reg_ac << 7) >> 7);
            reg_ac = (reg_ac >> 1) | (old_carry << 7); 
            break;
        }
        // Change based on memory in the future
        case ROR_Z: {
            uint8_t address = pc_read();
            uint8_t operand = mem_read(address);
            uint8_t old_carry = get_carry();
            set_carry((operand << 1) >> 7);
            operand = (operand >> 1) | (old_carry << 7); 
            mem_write(address, operand);
            break;
        }
        case ROR_ZX: {
            uint8_t address = pc_read();
            uint8_t operand = mem_read(address + reg_x);
            cycles++;
            uint8_t old_carry = get_carry();
            set_carry((operand << 1) >> 7);
            operand = (operand >> 1) | (old_carry << 7); 
            mem_write(address, operand);
            break;
        }
        case ROR_A: {
            uint8_t address = pc_read2();
            uint8_t operand = mem_read(address);
            uint8_t old_carry = get_carry();
            set_carry((operand << 1) >> 7);
            operand = (operand >> 1) | (old_carry << 7); 
            mem_write(address, operand);
            break;
        }
        case ROR_AX: {
            uint16_t address = pc_read2();
            uint16_t shift = address + reg_x;
            
            // if adding the value of register x crosses a page boundary, take another cycle
            PAGE_SHIFT(shift, address);
            uint8_t operand = mem_read(shift);
            uint8_t old_carry = get_carry();
            set_carry((operand << 1) >> 7);
            operand = (operand >> 1) | (old_carry << 7); 
            mem_write(address, operand);
            break;
        }
        
        /////////
        // CMP //
        /////////

        // Compare sets flags as if a subtraction had been carried out. If the value in the accumulator
        // is equal or greater than the compared value, the Carry will be set. The equal (Z) 
        // and negative (N) flags will be set based on equality or lack thereof and the
        // sign (i.e. A>=$80) of the accumulator.

        case CMP_I: {
            cmp(reg_ac, imm());
            break;
        }
        case CMP_Z: {
            cmp(reg_ac, zp());
            break;
        }
        case CMP_ZX: {
            cmp(reg_ac, zp_x());
            break;
        }
        case CMP_A: {
            cmp(reg_ac, abs());
            break;
        }
        case CMP_AX: {
            cmp(reg_ac, abs_x());
            break;
        }
        case CMP_AY: {
            cmp(reg_ac, abs_y());
            break;
        }
        case CMP_IX: {
            cmp(reg_ac, ind_x());
            break;
        }
        case CMP_IY: {
            cmp(reg_ac, ind_y());
            break;
        }

        /////////
        // CPX //
        /////////

        // This instruction compares the contents of the X register with another
        // memory held value and sets the zero and carry flags as appropriate.

        case CPX_I: {
            cmp(reg_x, imm());
            break;
        }
        case CPX_Z: {
            cmp(reg_x, zp());
            break;
        }
        case CPX_A: {
            cmp(reg_x, abs());
            break;
        }

        /////////
        // CPY //
        /////////

        // This instruction compares the contents of the Y register with another
        // memory held value and sets the zero and carry flags as appropriate.
        case CPY_I: {
            cmp(reg_y, imm());
            break;
        }
        case CPY_Z: {
            cmp(reg_y, zp());
            break;
        }
        case CPY_A: {
            cmp(reg_y, abs());
            break;
        }
        case BCC: {
            b(!get_carry());
            break;
        }
        case BCS: {
            b(get_carry());
            break;
        }
        case BEQ: {
            b(get_zero());
            break;
        }
        case BNE: {
            b(!get_zero());
            break;
    
        }
        case BVC: {
            b(!get_overflow());
            break;
        }
        case BVS: {
            b(get_overflow());
            break;
        }
        case BPL: {
            b(!get_negative());
            break;
        }
        case BMI: {
            b(get_negative());
            break;
        }
        case JMP_I: {
            uint16_t operand = pc_read2();
            uint16_t nextMem = ((operand + 1) % 0x100) + ((operand >> 8) << 8);
    
            reg_pc = mem_read(operand) + (mem_read(nextMem) << 8);
            break;
        }
        case JMP_A: {
            reg_pc = pc_read2();
            break;
        }
        case JSR: {
            uint16_t operand = pc_read2() - 1;
            std::cout << "pc: " << reg_pc << " ";
            push16(reg_pc - 1);
            cycles++;
            reg_pc = operand + 1;
            break;
        }
        case RTI: {
            reg_p = pop();
            reg_pc = pop16();
            set_break(0);
            set_one(1);
            cycles += 2;
            break;
        }
        case RTS: {
            uint16_t newaddr = pop16() + 1;
            std::cout << "new addr: " << newaddr << " ";
            reg_pc = newaddr;
            cycles += 3;
            break;
        }
        case SEC: {
            cycles++;
            set_carry(1);
            break;
        }
        case SED: {
            cycles++;
            set_decimal(1);
            break;
        }
        case SEI: {
            cycles++;
            set_interrupt(1);
            break;
        }
        case CLD: {
            cycles++;
            set_decimal(0);
            break;
        }
        case CLI: {
            cycles++;
            set_interrupt(0);
            break;
        }
        case CLC: {
            cycles++;
            set_carry(0);
            break;
        }
        case CLV: {
            cycles++;
            set_overflow(0);
            break;
        }
        case PHA: {
            push(reg_ac);
            cycles++;
            break;
        }
        case PHP: {
            push(reg_p);
            cycles++;
            break;
        }
        case PLA: {
            reg_ac = pop();
            check_nz(reg_ac);
            cycles += 2;
            break;
        }
        case PLP: {
            reg_p = pop();
            set_break(0);
            set_one(1);
            cycles += 2;
            break;
        }
        case NOP:
        case NOP_2: {
            cycles++;
            break;
        }
        default: {
            std::cout << "invalid opcode: " << std::hex << unsigned(opcode) << std::endl;
            std::cout << "byte 02: " << std::hex << unsigned(memory->mem_read(2)) << std::endl;
            return ERROR;
        }
        // add all the other opcodes
    }
    
    //std::cout << "opcode: " << std::hex << unsigned(opcode) << std::endl;
    
    if((reg_p & 32) >> 5 == 0) {
        //exit(0);
    }
    
    std::cout << "cycles: " << std::dec << unsigned(total_cycles) << std::endl;
    total_cycles += cycles;
    return cycles;
}

uint8_t CPU::imm() {
    return pc_read();
}

uint8_t CPU::zp() {
    uint8_t address = pc_read();
    return mem_read(address);
}

uint8_t CPU::zp_x() {
    uint8_t address = pc_read();
    cycles++;
    return mem_read(address + reg_x);
}

uint8_t CPU::zp_y() {
    uint8_t address = pc_read();
    cycles++;
    return mem_read(address + reg_y);
}

uint8_t CPU::abs() {
    uint16_t address = pc_read2();
    return mem_read(address);
}

uint8_t CPU::abs_x() {
    uint16_t address = pc_read2();
    uint16_t shift = address + reg_x;
    
    // if adding the value of register x crosses a page boundary, take another cycle
    PAGE_SHIFT(shift, address);
    return mem_read(shift);
}

uint8_t CPU::abs_y() {
    uint16_t address = pc_read2();
    uint16_t shift = address + reg_y;
    
    // if adding the value of register x crosses a page boundary, take another cycle
    PAGE_SHIFT(shift, address);
    
    return mem_read(shift);
}

uint8_t CPU::ind_x() {
    uint8_t operand = pc_read();
    uint16_t address = mem_read2((uint8_t) (operand + reg_x));
    uint8_t value = mem_read(address);
    cycles++;
    
    return value;
}

uint8_t CPU::ind_y() {
    uint8_t operand = pc_read();
    uint16_t address = mem_read2(operand);
    uint16_t shift = address + reg_y;
    
    PAGE_SHIFT(shift, address);
    
    uint8_t value = mem_read(shift);
    
    return value;
}

void CPU::lda(uint8_t operand) {
    check_nz(operand);
    reg_ac = operand;
}

void CPU::ldx(uint8_t operand) {
    check_nz(operand);
    reg_x = operand;
}

void CPU::ldy(uint8_t operand) {
    check_nz(operand);
    reg_y = operand;
}

void CPU::sbc(uint8_t operand) {
    adc(~operand);
}

void CPU::adc(uint8_t operand) {
    uint8_t sum = reg_ac + operand + get_carry();
    set_carry(sum < operand);
    set_overflow(~(reg_ac ^ operand) & (reg_ac ^ sum) & 0x80);
    check_nz(sum);
    reg_ac = sum;
}

void CPU::cmp(uint8_t reg, uint8_t mem) {
    if (reg < mem) {
        set_zero(0);
        set_carry(0);
    } else if (reg == mem) {
        set_zero(1);
        set_carry(1);
    } else {
        set_zero(0);
        set_carry(1);
    }
    set_negative(NEGATIVE(reg - mem));
}

void CPU::b(bool condition) {
    uint8_t operand = pc_read();
    if (condition) {
        cycles++;
        uint16_t new_pc = reg_pc + operand;
        PAGE_SHIFT(new_pc, reg_pc + 1);
        reg_pc = new_pc;
    }
}

void CPU::check_nz(uint8_t operand) {
    set_negative(NEGATIVE(operand));
    set_zero(ZERO(operand));
}

void CPU::page_shift(uint16_t shift, uint16_t addr) {
    if ((shift >> 8 ) > (addr >> 8)) {
        cycles++;
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

void CPU::set_one(bool value) {
    reg_p &= 0xdf;
    reg_p |= (((uint8_t) value) << 5);
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

uint8_t CPU::mem_read(uint64_t index) {
    cycles++;
    return memory->mem_read(index);
}

uint16_t CPU::mem_read2(uint64_t index) {
    cycles += 2;
    return memory->mem_read2(index);
}

void CPU::mem_write(uint64_t index, uint8_t value) {
    cycles++;
    memory->mem_write(index, value);
}

uint8_t CPU::pc_read() {
    uint8_t data = mem_read(reg_pc);
    reg_pc++;
    std::cout << std::hex << unsigned(data) << " ";
    return data;
}

uint16_t CPU::pc_read2() {
    uint16_t data = pc_read() + (((uint16_t) pc_read()) << 8);
    return data;
}

void CPU::push(uint8_t value) {
    mem_write(((uint16_t) reg_s) + 0x100, value);
    reg_s--;
}

void CPU::push16(uint16_t value) {
    push((uint8_t) (value >> 8));
    push((uint8_t) value);
}

uint8_t CPU::pop() {
    reg_s++;
    uint8_t data = mem_read(((uint16_t) reg_s) + 0x100);
    return data;
}

uint16_t CPU::pop16() {
    uint16_t data = pop();
    data += ((uint16_t) pop()) << 8;
    return data;
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