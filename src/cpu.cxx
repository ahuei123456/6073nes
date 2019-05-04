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

        /////////
        // AND //
        /////////

        //A logical AND is performed, bit by bit, on the accumulator contents 
        // using the contents of a byte of memory.

        case AND_I: {
            uint16_t operand = reg_ac & pc_read();
            set_negative(NEGATIVE(operand));//set negative
            set_zero(NEGATIVE(operand));//set zero
            reg_ac = (uint8_t) operand;
            break;
        }
        case AND_Z: {
            uint8_t address = pc_read();
            uint16_t operand = reg_ac & mem_read(address);
            set_negative(NEGATIVE(operand));//set negative
            set_zero(NEGATIVE(operand));//set zero
            reg_ac = (uint8_t) operand;
            break;
        }
        case AND_ZX: {
            uint8_t address = pc_read();
            uint16_t operand = reg_ac & mem_read(address + reg_x);
            set_negative(NEGATIVE(operand));//set negative
            set_zero(NEGATIVE(operand));//set zero
            reg_ac = (uint8_t) operand;
            break;
        }
        case AND_A: {
            uint8_t address = pc_read2();
            uint16_t operand = reg_ac & mem_read(address);
            set_negative(NEGATIVE(operand));//set negative
            set_zero(NEGATIVE(operand));//set zero
            reg_ac = (uint8_t) operand;
            break;
        }
        case AND_AX: {
            uint16_t address = pc_read2();
            uint16_t shift = address + reg_x;
            
            // if adding the value of register x crosses a page boundary, take another cycle
            PAGE_SHIFT(shift, address);
            uint16_t operand = reg_ac & mem_read(shift);
            set_negative(NEGATIVE(operand));//set negative
            set_zero(NEGATIVE(operand));//set zero
            reg_ac = (uint8_t) operand;
            break;
        }
        case AND_AY: {
            uint16_t address = pc_read2();
            uint16_t shift = address + reg_y;
            
            // if adding the value of register x crosses a page boundary, take another cycle
            PAGE_SHIFT(shift, address);
            uint16_t operand = reg_ac & mem_read(shift);
            set_negative(NEGATIVE(operand));//set negative
            set_zero(NEGATIVE(operand));//set zero
            reg_ac = (uint8_t) operand;
            break;
        }
        case AND_IX: {
            uint8_t op1 = pc_read();
            uint16_t address = mem_read2((uint8_t) (operand + reg_x));
            uint8_t value = mem_read(address);
            cycles++;

            uint16_t operand = reg_ac & value;
            set_negative(NEGATIVE(operand));//set negative
            set_zero(NEGATIVE(operand));//set zero
            reg_ac = (uint8_t) operand;
            break;
        }
        case AND_IY: {
            uint8_t op1 = pc_read();
            uint16_t address = mem_read2(op1);
            uint16_t shift = address + reg_y;
            
            PAGE_SHIFT(shift, address);
            
            uint8_t value = mem_read(address + reg_y);
            //
            uint16_t operand = reg_ac & value;
            set_negative(NEGATIVE(operand));//set negative
            set_zero(NEGATIVE(operand));//set zero
            reg_ac = (uint8_t) operand;
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

            set_carry(opreand >> 7);
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

            set_zero(ZERO(reg_ac & LC_ADDRESS));//????
            set_negative(pc_read() >> 7);
            set_overflow(pc_read()>> 6);
            break;
        }
        case BIT_A: {
            set_zero(reg_ac & LC_ADDRESS);//????
            set_negative(pc_read() >> 7);
            set_overflow(pc_read() >> 6);
            break;
        }

        /////////
        // EOR //
        /////////

        // EOR (Exclusive OR Memory With Accumulator) performs a logical XOR on the operand 
        // and the accumulator and stores the result in the accumulator. 
        // This opcode is similar in function to AND and ORA.

        case EOR_I; {
            uint16_t operand = reg_ac ^ pc_read();
            set_negative(NEGATIVE(operand));
            set_zero(ZERO(operand));

            reg_ac = (uint8_t) operand;
        }
        case EOR_Z; {
            uint8_t address = pc_read();
            uint8_t operand = reg_ac ^ mem_read(address);
            
            set_negative(NEGATIVE(operand));
            set_zero(ZERO(operand));

            reg_ac = (uint8_t) operand;
        }
        case EOR_ZX; {
            uint8_t address = pc_read();
            uint8_t operand = reg_ac ^ mem_read(address + reg_x);
            cycles++;

            set_negative(NEGATIVE(operand));
            set_zero(ZERO(operand));

            reg_ac = (uint8_t) operand;
        }
        case EOR_A; {
            uint16_t address = pc_read2();
            uint8_t operand = reg_ac ^ mem_read(address);
            
            set_negative(NEGATIVE(operand));
            set_zero(ZERO(operand));

            reg_ac = (uint8_t) operand;
        }
        case EOR_AX; {
            uint16_t address = pc_read2();
            uint16_t shift = address + reg_x;
            
            // if adding the value of register x crosses a page boundary, take another cycle
            PAGE_SHIFT(shift, address);
            
            uint8_t operand = reg_ac ^ mem_read(shift);
            set_negative(NEGATIVE(operand));
            set_zero(ZERO(operand));
            reg_ac = (uint8_t) operand;
        }
        case EOR_AY; {
            uint16_t address = pc_read2();
            uint16_t shift = address + reg_y;
            
            // if adding the value of register x crosses a page boundary, take another cycle
            PAGE_SHIFT(shift, address);
            
            uint8_t operand = reg_ac ^ mem_read(shift);
            set_negative(NEGATIVE(operand));
            set_zero(ZERO(operand));

            reg_ac = (uint8_t) operand;
        }
        case EOR_IX; {
            uint8_t operand = pc_read();
            uint16_t address = mem_read2((uint8_t) (operand + reg_x));
            uint8_t value = reg_ac ^ mem_read(address);
            cycles++;
            set_negative(NEGATIVE(value));
            set_zero(ZERO(operand));
            reg_ac = (uint8_t) value;
        }
        case EOR_IY; {
            uint8_t operand = pc_read();
            uint16_t address = mem_read2(operand);
            uint16_t shift = address + reg_y;
            
            PAGE_SHIFT(shift, address);
            
            uint8_t value = reg_ac ^ mem_read(address + reg_y);
            set_negative(NEGATIVE(value));
            set_zero(ZERO(value));
            
            reg_ac = (uint8_t) value;
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
        
        case ORA_I; {
            uint16_t operand = reg_ac | pc_read();
            set_negative(NEGATIVE(operand));
            set_zero(ZERO(operand));

            reg_ac = (uint8_t) operand;
        }
        case ORA_Z; {
            uint8_t address = pc_read();
            uint8_t operand = reg_ac | mem_read(address);
            
            set_negative(NEGATIVE(operand));
            set_zero(ZERO(operand));

            reg_ac = (uint8_t) operand;
        }
        case ORA_ZX; {
            uint8_t address = pc_read();
            uint8_t operand = reg_ac | mem_read(address + reg_x);
            cycles++;

            set_negative(NEGATIVE(operand));
            set_zero(ZERO(operand));

            reg_ac = (uint8_t) operand;
        }
        case ORA_A; {
            uint16_t address = pc_read2();
            uint8_t operand = reg_ac | mem_read(address);
            
            set_negative(NEGATIVE(operand));
            set_zero(ZERO(operand));

            reg_ac = (uint8_t) operand;
        }
        case ORA_AX; {
            uint16_t address = pc_read2();
            uint16_t shift = address + reg_x;
            
            // if adding the value of register x crosses a page boundary, take another cycle
            PAGE_SHIFT(shift, address);
            
            uint8_t operand = reg_ac | mem_read(shift);
            set_negative(NEGATIVE(operand));
            set_zero(ZERO(operand));
            reg_ac = (uint8_t) operand;
        }
        case ORA_AY; {
            uint16_t address = pc_read2();
            uint16_t shift = address + reg_y;
            
            // if adding the value of register x crosses a page boundary, take another cycle
            PAGE_SHIFT(shift, address);
            
            uint8_t operand = reg_ac | mem_read(shift);
            set_negative(NEGATIVE(operand));
            set_zero(ZERO(operand));

            reg_ac = (uint8_t) operand;
        }
        case ORA_IX; {
            uint8_t operand = pc_read();
            uint16_t address = mem_read2((uint8_t) (operand + reg_x));
            uint8_t value = reg_ac | mem_read(address);
            cycles++;
            set_negative(NEGATIVE(value));
            set_zero(ZERO(operand));
            reg_ac = (uint8_t) value;
        }
        case ORA_IY; {
            uint8_t operand = pc_read();
            uint16_t address = mem_read2(operand);
            uint16_t shift = address + reg_y;
            
            PAGE_SHIFT(shift, address);
            
            uint8_t value = reg_ac | mem_read(address + reg_y);
            set_negative(NEGATIVE(value));
            set_zero(ZERO(value));
            
            reg_ac = (uint8_t) value;
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

        ///////////////////
        //// REGISTERS ////
        ///////////////////

        /////////
        // CLC //
        /////////

        // CLC (Clear Carry Flag) clears the Carry Flag in the Processor Status Register by
        // setting the 0th bit 0. To set the carry flag, use SEC. 
        // Clearing the carry flag should be done prior to any instruction 
        // that might set it where you might need to read the carry flag's value after the instruction.

        case CLC: {
            set_carry((0));
            break;
        }

        /////////
        // CLD //
        /////////

        // CLD (Clear Decimal Flag) clears the Decimal Flag in the Processor Status Register
        // by setting the 3rd bit 0. To set the decimal flag, use SED.
        // Even though the NES doesn't use decimal mode, the opcodes to clear and
        // set the flag do work, so if you need to store a bit, this acts as a free space.

        case CLD: {
            set_decimal((0));
            break;
        }

        /////////
        // CLI //
        /////////

        // CLI (Clear Interrupt Disable Flag) clears the Interrupt Flag in the
        // Processor Status Register by setting the 2nd bit 0. To set the interrupt disable flag, use SEI.

        case CLI: {
            set_interrupt((0));
            break;
        }

        /////////
        // CLV //
        /////////

        // CLV (Clear Overflow Flag) clears the Overflow Flag in the Processor
        // Status Register by setting the 6th bit 0. There is no opcode for setting the overflow flag.

        case CLV: {
            set_overflow((0));
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
            uint16_t operand = pc_read();
            if(reg_ac >= operand){
                set_carry(1);
            }
            if(reg_ac == operand){
                set_zero(1);
            }
            set_negative(NEGATIVE(reg_ac));
            // IF negative set negative bit, but Im not sure how to do that
            break;
        }
        case CMP_Z: {
            uint8_t address = pc_read();
            uint8_t operand = mem_read(address);
            if(reg_ac >= operand){
                set_carry(1);
            }
            if(reg_ac == operand){
                set_zero(1);
            }
            set_negative(NEGATIVE(operand));
            // IF negative set negative bit, but Im not sure how to do that
            break;
        }
        case CMP_ZX: {
            uint8_t address = pc_read();
            uint8_t operand = mem_read(address + reg_x);
            cycles++;
            if(reg_ac >= operand){
                set_carry(1);
            }
            if(reg_ac == operand){
                set_zero(1);
            }
            set_negative(NEGATIVE(operand));
            // IF negative set negative bit, but Im not sure how to do that
            break;
        }
        case CMP_A: {
            uint8_t address = pc_read2();
            uint8_t operand = mem_read(address);
            if(reg_ac >= operand){
                set_carry(1);
            }
            if(reg_ac == operand){
                set_zero(1);
            }
            set_negative(NEGATIVE(operand));
            // IF negative set negative bit, but Im not sure how to do that
            break;
        }
        case CMP_AX: {
            uint16_t address = pc_read2();
            uint16_t shift = address + reg_x;
            
            // if adding the value of register x crosses a page boundary, take another cycle
            PAGE_SHIFT(shift, address);
            
            uint8_t operand = mem_read(shift);
            if(reg_ac >= operand){
                set_carry(1);
            }
            if(reg_ac == operand){
                set_zero(1);
            }
            set_negative(NEGATIVE(operand));
            // IF negative set negative bit, but Im not sure how to do that
            break;
        }
        case CMP_AY: {
            uint16_t address = pc_read2();
            uint16_t shift = address + reg_y;
            
            // if adding the value of register x crosses a page boundary, take another cycle
            PAGE_SHIFT(shift, address);
            
            uint8_t operand = mem_read(shift);
            if(reg_ac >= operand){
                set_carry(1);
            }
            if(reg_ac == operand){
                set_zero(1);
            }
            set_negative(NEGATIVE(operand));
            // IF negative set negative bit, but Im not sure how to do that
            break;
        }
        case CMP_IX: {
            uint8_t operand = pc_read();
            uint16_t address = mem_read2((uint8_t) (operand + reg_x));
            uint8_t value = mem_read(address);
            cycles++;
            if(reg_ac >= value){
                set_carry(1);
            }
            if(reg_ac == value){
                set_zero(1);
            }
            set_negative(NEGATIVE(value));
            // IF negative set negative bit, but Im not sure how to do that
            break;
        }
        case CMP_IY: {
            uint8_t operand = pc_read();
            uint16_t address = mem_read2(operand);
            uint16_t shift = address + reg_y;
            
            PAGE_SHIFT(shift, address);
            
            uint8_t value = mem_read(address + reg_y);
            if(reg_ac >= value){
                set_carry(1);
            }
            if(reg_ac == value){
                set_zero(1);
            }
            set_negative(NEGATIVE(value));
            // IF negative set negative bit, but Im not sure how to do that
            break;
        }

        /////////
        // CPX //
        /////////

        // This instruction compares the contents of the X register with another
        // memory held value and sets the zero and carry flags as appropriate.

        case CPX_I: {
            uint8_t operand = pc_read();
            if(reg_x >= operand){
                set_carry(1);
            }
            if(reg_x == operand){
                set_zero(1);
            }
            set_negative(NEGATIVE(operand));
            // IF negative set negative bit, but Im not sure how to do that
        }
        case CPX_Z: {
            uint8_t address = pc_read();
            uint8_t operand = mem_read(address);
            if(reg_x >= operand){
                set_carry(1);
            }
            if(reg_x == operand){
                set_zero(1);
            }
            set_negative(NEGATIVE(operand));
            // IF negative set negative bit, but Im not sure how to do that
        }
        case CPX_A: {
            uint8_t address = pc_read2();
            uint8_t operand = mem_read(address);
            if(reg_x >= operand){
                set_carry(1);
            }
            if(reg_x == operand){
                set_zero(1);
            }
            set_negative(NEGATIVE(operand));
            // IF negative set negative bit, but Im not sure how to do that
        }

        /////////
        // CPY //
        /////////

        // This instruction compares the contents of the Y register with another
        // memory held value and sets the zero and carry flags as appropriate.
        case CPY_I: {
            uint8_t operand = pc_read();
            if(reg_y >= operand){
                set_carry(1);
            }
            if(reg_y == operand){
                set_zero(1);
            }
            set_negative(NEGATIVE(operand));
            // IF negative set negative bit, but Im not sure how to do that
        }
        case CPY_Z: {
            uint8_t address = pc_read();
            uint8_t operand = mem_read(address);
            if(reg_y >= operand){
                set_carry(1);
            }
            if(reg_y == operand){
                set_zero(1);
            }
            set_negative(NEGATIVE(operand));
            // IF negative set negative bit, but Im not sure how to do that
        }
        case CPY_A: {
            uint8_t address = pc_read2();
            uint8_t operand = mem_read(address);
            if(reg_y >= operand){
                set_carry(1);
            }
            if(reg_y == operand){
                set_zero(1);
            }
            set_negative(NEGATIVE(operand));
            // IF negative set negative bit, but Im not sure how to do that
        }
        /////////
        // SEC //
        /////////

        // SEC (Set Carry Flag) sets the Carry Flag in the Processor Status Register 
        // by setting the 0th bit 1. To clear the carry flag, use CLC.

        case SEC: {
            set_carry((1));
            break;
        }

        /////////
        // SED //
        /////////

        // SED (Set Decimal Flag) set the Decimal Flag in the Processor Status Register
        // by setting the 3rd bit 1. To clear the decimal flag, use CLD.
        // Even though the NES doesn't use decimal mode, the opcodes to clear
        // and set the flag do work, so if you need to store a bit, this acts as a free space.

        case SED: {
            set_carry((1));
            break;
        }

        /////////
        // SEI //
        /////////

        // SEI (Set Interrupt Disable Flag) sets the Interrupt Flag in the Processor Status Register
        // by setting the 2nd bit 1. To clear the interrupt disable flag, use CLI.

        case SEI: {
            set_interrupt((1));
            break;
        }
    }
    
    std::cout << "opcode: " << std::hex << unsigned(opcode) << std::endl;
    std::cout << "cycles: " << unsigned(cycles) << std::endl;
}
//Set negative: "Clears the Negative Flag if the Operand is $#00-7F, otherwise sets it."?????
//
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

// uint8_t CPU:: get_s() {
//     return reg_s;
// }
