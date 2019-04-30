#include "cpu.hpp"

CPU::CPU(std::shared_ptr<Mem> memory) {
    this->memory = memory;
    
    reg_s = 0xFF;
    reg_pc = memory->reset_vector();
}

void CPU::execute() {
    cycles = 0;
    uint8_t opcode = pc_read();
    
    switch (opcode) {
        case LDA_I: {
            uint16_t operand = pc_read();
            
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
            uint8_t operand = pc_read();
            if (!get_carry()){
                reg_pc += operand;
            }
            break;
        }
        case BCS: {
            uint8_t operand = pc_read();
            if (get_carry()){
                reg_pc += operand;
            }
            break;
        }
        case BEQ: {
            uint8_t operand = pc_read();
            if (get_zero()) {
                reg_pc += operand;
            }	
            break;
        }
        case BNE: {
            uint8_t operand = pc_read();
            if (!get_zero()) {
                reg_pc += operand;
            }
            break;
    
        }
        case BVC: {
            uint8_t operand = pc_read();
            if (!get_overflow()) {
                reg_pc += operand;
            }
            break;
        }
        case BVS: {
            uint8_t operand = pc_read();
            if (get_overflow()) {
                reg_pc += operand;
            }
            reg_pc += 2;
            break;
        }
        case BPL: {
            uint8_t operand = pc_read();
            if (!get_negative()) {
                reg_pc += operand;
            }
            break;
        }
        case BMI: {
            uint8_t operand = pc_read();
            if (get_negative()) {
                reg_pc += operand;
            }
            break;
        }
        case JMP_I: {
            uint16_t operand = pc_read();
            uint16_t nextMem = ((operand + 1) % 0xFF) + (operand >> 8);
    
            reg_pc = mem_read(operand) + (mem_read(nextMem) << 8);
            break;
        }
        case JMP_A: {
            reg_pc = pc_read2();
            break;
        }
        case JSR: {
            uint16_t operand = pc_read2() - 1;
            push(operand >> 8);
            push(operand & 0xFF);
            reg_pc = operand + 1;	    
            break;
        }
        case RTI: {
            reg_p = pop();
            reg_pc = pop16();
            break;
        }
        case RTS: {
            reg_pc = pop16() + 1;
            break;
        }
        default: {
            break;
        }
        // add all the other opcodes
    }
    
    std::cout << "opcode: " << std::hex << unsigned(opcode) << std::endl;
    std::cout << "cycles: " << unsigned(cycles) << std::endl;
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
    return data;
}

uint16_t CPU::pc_read2() {
    uint16_t data = mem_read2(reg_pc);
    reg_pc += 2;
    return data;
}

void CPU::push(uint8_t value) {
    memory->mem_write(((uint16_t) reg_s) + 0x100, value);
    reg_s--;
}

void CPU::push16(uint16_t value) {
    push((uint8_t) value);
    push((uint8_t) (value >> 8));
}

uint8_t CPU::pop() {
    uint8_t data = (uint8_t) memory->mem_read(((uint16_t) reg_s) + 0x100);
    reg_s++;
    return data;
}

uint16_t CPU::pop16() {
    uint16_t data = ((uint16_t) pop()) << 8;
    data += pop();
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
