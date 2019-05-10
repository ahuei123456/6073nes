#include "cpu.hpp"
#define DEBUG 1

void Inst::add_operand(uint8_t operand) {
    if (operand != opcode){
        operands.push_back(operand);
    }
}

std::string Inst::str() {
    std::stringstream buffer;
    
    buffer << std::setfill('0') << std::setw(4) << std::hex << std::uppercase << unsigned(reg_pc) << " ";
    
    buffer << "Opcode: " << std::setw(2) << std::hex << unsigned(operands[0]) << " ";
    buffer << "Operands: ";
    
    uint8_t counter = 0;
    for (auto i: operands) {
        if (i != opcode) {
            counter++;
            buffer << std::setw(2) << std::hex << std::uppercase << unsigned(i) << " ";
        }
    }
    
    if (counter == 0) buffer << "      ";
    else if (counter == 1) buffer << "   ";
    buffer << "              ";
    
    buffer << "A: " << std::setw(2) << std::hex << unsigned(reg_ac) << " ";
    buffer << "X: " << std::setw(2) << std::hex << unsigned(reg_x) << " ";
    buffer << "Y: " << std::setw(2) << std::hex << unsigned(reg_y) << " ";
    buffer << "P: " << std::setw(2) << std::hex << unsigned(reg_p) << " ";
    buffer << "S: " << std::setw(2) << std::hex << unsigned(reg_s) << " ";
    buffer << "CYC: " << std::dec << unsigned(total_cycles) << " ";
    
    return buffer.str();
}

CPU::CPU(std::shared_ptr<Mem> memory) {
    this->memory = memory;
    total_cycles = 7;
    reg_ac = 0;
    reg_x = 0;
    reg_y = 0;
    reg_s = 0xFD;
    reg_p = 0x24;
    reg_pc = memory->reset_vector();
    //::cout << std::hex << unsigned(reg_pc) << std::endl;
}

void CPU::set_debug() {
    inst.reg_pc = reg_pc;
    inst.reg_ac = reg_ac;
    inst.reg_x = reg_x;
    inst.reg_y = reg_y;
    inst.reg_p = reg_p;
    inst.reg_s = reg_s;
    inst.total_cycles = total_cycles;
    inst.operands.clear();
}

uint16_t CPU::execute() {
    cycles = 0;
    
    if (reg_pc == 0xdb65) {
        cycles = 0;
    }
    
    set_debug();
    
    if (memory->read_nmi()) {
        std::cout << "nmi" << std::endl;
        memory->set_nmi(0);
        push16(reg_pc);
        push(reg_s);
        
        reg_pc = memory->nmi_vector();
        cycles += 2;
    }
    
    uint8_t opcode = pc_read();
    
    inst.opcode = opcode;
    
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
            sta(a_zp());
            break;
        }
        case STA_ZX: {
            sta(a_zp_x());
            break;
        }
        case STA_A: {
            sta(a_abs());
            break;
        }
        case STA_AX: {
            sta(a_abs_x());
            cycles = 5;
            break;
        }
        case STA_AY: {
            sta(a_abs_y());
            cycles = 5;
            break;
        }
        // check over everything after this
        case STA_IX: {
            sta(a_ind_x());
            break;
        }
        case STA_IY: {
            sta(a_ind_y());
            cycles = 6;
            break;
        }
        // here
        case STX_Z: {
            stx(a_zp());
            break;
        }
        case STX_ZY: {
            stx(a_zp_y());
            break;
        }
        case STX_A: {
            stx(a_abs());
            break;
        }
        case STY_Z: {
            sty(a_zp());
            break;
        }
        case STY_ZX: {
            sty(a_zp_x());
            break;
        }
        case STY_A: {
            sty(a_abs());
            break;
        }
        case TAX: {
            reg_x = reg_ac;
            check_nz(reg_x);
            cycles++;
            break;
        }
        case TAY: {
            reg_y = reg_ac;
            check_nz(reg_y);
            cycles++;
            break;
        }
        case TSX: {
            reg_x = reg_s;
            check_nz(reg_x);
            cycles++;
            break;
        }
        case TXA: {
            reg_ac = reg_x;
            check_nz(reg_ac);
            cycles++;
            break;
        }
        case TXS: {
            reg_s = reg_x;
            cycles++;
            break;
        }
        case TYA: {
            reg_ac = reg_y;
            check_nz(reg_ac);
            cycles++;
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
            dec(a_zp());
            break;
        }
        case DEC_ZX: {
            dec(a_zp_x());
            break;
        }
        case DEC_A: {
            dec(a_abs());
            break;
        }
        case DEC_AX: {
            dec(a_abs_x());
            cycles++;
            break;
        }
        case INC_Z: {
            inc(a_zp());
            break;
        }
        case INC_ZX: {
            inc(a_zp_x());
            break;
        }
        case INC_A: {
            inc(a_abs());
            break;
        }
        case INC_AX: {
            inc(a_abs_x());
            cycles++;
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
            aan(imm());
            break;
        }
        case AND_Z: {
            aan(zp());
            break;
        }
        case AND_ZX: {
            aan(zp_x());
            break;
        }
        case AND_A: {
            aan(abs());
            break;
        }
        case AND_AX: {
            aan(abs_x());
            break;
        }
        case AND_AY: {
            aan(abs_y());
            break;
        }
        case AND_IX: {
            aan(ind_x());
            break;
        }
        case AND_IY: {
            aan(ind_y());
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
            reg_ac = asl(reg_ac);
            break;
        }
        case ASL_Z: {
            asl_m(a_zp()); 
            break;
        }
        case ASL_ZX: {
            asl_m(a_zp_x());
            break;
        }
        case ASL_A: {
            asl_m(a_abs());
            break;
        }
        case ASL_AX: {
            asl_m(a_abs_x());
            cycles++;
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
            eor(imm());
            break;
        }
        case EOR_Z: {
            eor(zp());
            break;
        }
        case EOR_ZX: {
            eor(zp_x());
            break;
        }
        case EOR_A: {
            eor(abs());
            break;
        }
        case EOR_AX: {
            eor(abs_x());
            break;
        }
        case EOR_AY: {
            eor(abs_y());
            break;
        }
        case EOR_IX: {
            eor(ind_x());
            break;
        }
        case EOR_IY: {
            eor(ind_y());
            break;
        }
        
        /////////
        // LSR //
        /////////

        // Each of the bits in A or M is shift one place to the right.
        // The bit that was in bit 0 is shifted into the carry flag. Bit 7 is set to zero.

        case LSR_AC: {
            reg_ac = lsr(reg_ac);
            break;
        }
        case LSR_Z: {
            lsr_m(a_zp());
            break;
        }
        case LSR_ZX: {
            lsr_m(a_zp_x());
            break;
        }
        case LSR_A: {
            lsr_m(a_abs());
            break;
        }
        case LSR_AX: {
            lsr_m(a_abs_x());
            cycles++;
            break;
        }

        /////////
        // ORA //
        /////////

        // ORA (Or Memory With Accumulator) performs a logical OR on the operand and the accumulator
        // and stores the result in the accumulator. This opcode is similar in function to AND and EOR.
        
        case ORA_I: {
            ora(imm());
            break;
        }
        case ORA_Z: {
            ora(zp());
            break;
        }
        case ORA_ZX: {
            ora(zp_x());
            break;
        }
        case ORA_A: {
            ora(abs());
            break;
        }
        case ORA_AX: {
            ora(abs_x());
            break;
        }
        case ORA_AY: {
            ora(abs_y());
            break;
        }
        case ORA_IX: {
            ora(ind_x());
            break;
        }
        case ORA_IY: {
            ora(ind_y());
            break;
        }

        /////////
        // ROL //
        /////////

        // ROL shifts all bits left one position. 
        // The Carry is shifted into bit 0 and the original bit 7 is shifted into the Carry.

        case ROL_AC: {
            reg_ac = rol(reg_ac);
            break; 
        }
        // Change based on memory in the future
        case ROL_Z: {
            rol_m(a_zp());
            break;
        }
        case ROL_ZX: {
            rol_m(a_zp_x());
            break;
        }
        case ROL_A: {
            rol_m(a_abs());
            break;
        }
        case ROL_AX: {
            rol_m(a_abs_x());
            cycles++;
            break;
        }

        /////////
        // ROR //
        /////////

        // Move each of the bits in either A or M one place to the right. 
        // Bit 7 is filled with the current value of the carry flag whilst the old bit 0 becomes the new carry flag value.

        case ROR_AC: {
            reg_ac = ror(reg_ac);
            break;
        }
        // Change based on memory in the future
        case ROR_Z: {
            ror_m(a_zp());
            break;
        }
        case ROR_ZX: {
            ror_m(a_zp_x());
            break;
        }
        case ROR_A: {
            ror_m(a_abs());
            break;
        }
        case ROR_AX: {
            ror_m(a_abs_x());
            cycles++;
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
        case NOP_11A:
        case NOP_11B:
        case NOP_11C:
        case NOP_11D:
        case NOP_11E:
        case NOP_11F:
        case NOP_11G:
        case NOP_11H: {
            cycles++;
            break;
        }
        case NOP_21A: {
            pc_read();
            break;
        }
        case NOP_22A:
        case NOP_22B:
        case NOP_22C: {
            pc_read();
            cycles++;
            break;
        }
        case NOP_23A: 
        case NOP_23B:
        case NOP_23C:
        case NOP_23D:
        case NOP_23E:
        case NOP_23F: {
            pc_read();
            cycles += 2;
            break;
        }
        case NOP_33A: {
            pc_read2();
            cycles++;
            break;
        }
        case NOP_34A: 
        case NOP_34B:
        case NOP_34C:
        case NOP_34D:
        case NOP_34E:
        case NOP_34F: {
            abs_x();
            break;
        }
        default: {
            std::cout << "pc: " << std::hex << unsigned(reg_pc) << std::endl;
            std::cout << "invalid opcode: " << std::hex << unsigned(opcode) << std::endl;
            std::cout << "byte 02: " << std::hex << unsigned(memory->mem_read(2)) << std::endl;
            std::cout << "byte 03: " << std::hex << unsigned(memory->mem_read(3)) << std::endl;
            return ERROR;
        }
        // add all the other opcodes
    }
    
    //std::cout << "opcode: " << std::hex << unsigned(opcode) << std::endl;
    
    if((reg_p & 32) >> 5 == 0) {
        //exit(0);
    }
    total_cycles += cycles;
    return cycles;
}



uint8_t CPU::imm() {
    return pc_read();
}

uint8_t CPU::zp() {
    return mem_read(a_zp());
}

uint8_t CPU::zp_x() {
    return mem_read(a_zp_x());
}

uint8_t CPU::zp_y() {
    return mem_read(a_zp_y());
}

uint8_t CPU::abs() {
    return mem_read(a_abs());
}

uint8_t CPU::abs_x() {
    return mem_read(a_abs_x());
}

uint8_t CPU::abs_y() {
    return mem_read(a_abs_y());
}

uint8_t CPU::ind_x() {
    return mem_read(a_ind_x());
}

uint8_t CPU::ind_y() {
    return mem_read(a_ind_y());
}

uint8_t CPU::a_zp() {
    return pc_read();
}

uint8_t CPU::a_zp_x() {
    cycles++;
    return pc_read() + reg_x;
}

uint8_t CPU::a_zp_y() {
    cycles++;
    return pc_read() + reg_y;
}

uint16_t CPU::a_abs() {
    return pc_read2();
}

uint16_t CPU::a_abs_x() {
    uint16_t address = pc_read2();
    uint16_t shift = address + reg_x;
    
    // if adding the value of register x crosses a page boundary, take another cycle
    PAGE_SHIFT(shift, address);
    return shift;
}

uint16_t CPU::a_abs_y() {
    uint16_t address = pc_read2();
    uint16_t shift = address + reg_y;
    
    // if adding the value of register x crosses a page boundary, take another cycle
    PAGE_SHIFT(shift, address);
    return shift;
}

uint16_t CPU::a_ind_x() {
    uint8_t operand = pc_read();
    uint8_t low = mem_read((uint8_t) (operand + reg_x));
    uint8_t high = mem_read((uint8_t) (operand + reg_x + 1));
    uint16_t address = low + (((uint16_t) high) << 8);
    cycles++;
    return address;
}

uint16_t CPU::a_ind_y() {
    uint8_t operand = pc_read();
    uint8_t low = mem_read(operand);
    uint8_t high = mem_read((uint8_t) (operand + 1));
    uint16_t address = low + (((uint16_t) high) << 8);
    uint16_t shift = address + reg_y;
    
    PAGE_SHIFT(shift, address);
    return shift;
}

uint8_t CPU::lsr(uint8_t value) {
    cycles++;
    set_carry(value & 1);
    value >>= 1;
    check_nz(value);
    return value;
}

uint8_t CPU::asl(uint8_t value) {
    cycles++;
    set_carry((value & 0x80) >> 7);
    value <<= 1;
    check_nz(value);
    return value;
}

uint8_t CPU::ror(uint8_t value) {
    cycles++;
    bool bit_0 = (value & 1);
    value >>= 1;
    value += ((uint8_t) get_carry()) << 7;
    set_carry(bit_0);
    check_nz(value);
    return value;
}

uint8_t CPU::rol(uint8_t value) {
    cycles++;
    bool bit_7 = ((value & 0x80) >> 7);
    value <<= 1;
    value += (uint8_t) get_carry();
    set_carry(bit_7);
    check_nz(value);
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

void CPU::sta(uint16_t address) {
    mem_write(address, reg_ac);
}

void CPU::stx(uint16_t address) {
    mem_write(address, reg_x);
}

void CPU::sty(uint16_t address) {
    mem_write(address, reg_y);
}

void CPU::dec(uint16_t address) {
    cycles++;
    uint8_t value = mem_read(address);
    value--;
    check_nz(value);
    mem_write(address, value);
}

void CPU::inc(uint16_t address) {
    cycles++;
    uint8_t value = mem_read(address);
    value++;
    check_nz(value);
    mem_write(address, value);
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
        uint16_t new_pc = reg_pc + (int8_t) operand;
        PAGE_SHIFT(new_pc, reg_pc + 1);
        reg_pc = new_pc;
    }
}

void CPU::ora(uint8_t operand) {
    reg_ac |= operand;
    check_nz(reg_ac);
}

void CPU::eor(uint8_t operand) {
    reg_ac ^= operand;
    check_nz(reg_ac);
}

void CPU::aan(uint8_t operand) {
    reg_ac &= operand;
    check_nz(reg_ac);
}

void CPU::lsr_m(uint16_t address) {
    uint8_t value = mem_read(address);
    value = lsr(value);
    mem_write(address, value);
}

void CPU::asl_m(uint16_t address) {
    uint8_t value = mem_read(address);
    value = asl(value);
    mem_write(address, value);
}

void CPU::ror_m(uint16_t address) {
    uint8_t value = mem_read(address);
    value = ror(value);
    mem_write(address, value);
}

void CPU::rol_m(uint16_t address) {
    uint8_t value = mem_read(address);
    value = rol(value);
    mem_write(address, value);
}

void CPU::check_nz(uint8_t operand) {
    set_negative(NEGATIVE(operand));
    set_zero(ZERO(operand));
}

void CPU::page_shift(uint16_t shift, uint16_t addr) {
    if ((shift >> 8 ) != (addr >> 8)) {
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
    
    inst.add_operand(data);
    
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

uint8_t CPU::get_s() {
    return reg_s;
}

uint64_t CPU::get_cycle() {
    return total_cycles;
}

std::string CPU::get_inst() {
    return inst.str();
}