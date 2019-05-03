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

        /////////
        // AND //
        /////////

        //A logical AND is performed, bit by bit, on the accumulator contents 
        // using the contents of a byte of memory.

        case AND_I: {
            uint16_t operand = reg_ac & memory->mem_read(reg_pc + 1);
            //set negative
            //set zero
            reg_ac = (uint8_t) operand;
            break;
        }
        case AND_Z: {
            uint16_t operand = reg_ac & memory->mem_read(reg_pc + 1);
            //set negative
            //set zero
            reg_ac = (uint8_t) operand;
            break;
        }
        case AND_ZX: {
            uint16_t operand = reg_ac & memory->mem_read(reg_pc + 1);
            //set negative
            //set zero
            reg_ac = (uint8_t) operand;
            break;
        }
        case AND_A: {
            uint16_t operand = reg_ac & memory->mem_read(reg_pc + 1);
            //set negative
            //set zero
            reg_ac = (uint8_t) operand;
            break
        }
        case AND_AX: {
            uint16_t operand = reg_ac & memory->mem_read(reg_pc + 1);
            //set negative
            //set zero
            reg_ac = (uint8_t) operand;
            break;
        }
        case AND_AY: {
            uint16_t operand = reg_ac & memory->mem_read(reg_pc + 1);
            //set negative
            //set zero
            reg_ac = (uint8_t) operand;
        }
        case AND_IX: {
            uint16_t operand = reg_ac & memory->mem_read(reg_pc + 1);
            //set negative
            //set zero
            reg_ac = (uint8_t) operand;
        }
        case AND_IY: {
            uint16_t operand = reg_ac & memory->mem_read(reg_pc + 1);
            //set negative
            //set zero
            reg_ac = (uint8_t) operand;
        }

        /////////
        // ASL //
        /////////

        // This operation shifts all the bits of the accumulator or memory contents one bit left.
        // Bit 0 is set to 0 and bit 7 is placed in the carry flag.
        // The effect of this operation is to multiply the memory contents by 2
        // (ignoring 2's complement considerations), setting the carry if the result will not fit in 8 bits.

        case ASL_AC: {
            set_carry(reg_ac >> 7)
            reg_ac = reg_ac << 1;
        }
        case ASL_Z: {
            set_carry(reg_ac >> 7)
            reg_ac = reg_ac << 1;
        }
        case ASL_ZX: {
            set_carry(reg_ac >> 7)
            reg_ac = reg_ac << 1;
        }
        case ASL_A: {
            set_carry(reg_ac >> 7)
            //reg_ac = reg_ac << 1;
            memory = memory << 1;
        }
        case ASL_AX: {
            set_carry(reg_ac >> 7)
            //reg_ac = reg_ac << 1;
            memory = memory << 1;
        }

        /////////
        // BIT //
        /////////

        // BIT sets the Z flag as though the value in the address tested were ANDed with the accumulator.
        // The S and V flags are set to match bits 7 and 6 respectively in the value stored at the tested address.
        
        case BIT_Z: {

            set_zero(reg_ac & LC_ADDRESS);//????
            set_s((memory->mem_read(reg_pc + 1) >> 7);
            set_v((memory->mem_read(reg_pc + 1) >> 6);
        }
        case BIT_A: {
            set_zero(reg_ac & LC_ADDRESS);//????
            set_s((memory->mem_read(reg_pc + 1) >> 7);
            set_v((memory->mem_read(reg_pc + 1) >> 6);
        }

        /////////
        // EOR //
        /////////

        // EOR (Exclusive OR Memory With Accumulator) performs a logical XOR on the operand 
        // and the accumulator and stores the result in the accumulator. 
        // This opcode is similar in function to AND and ORA.

        case EOR_I; {
            uint16_t operand = reg_ac ^ memory->mem_read(reg_pc + 1);
            if(operand == #00-7F)
                set_negative(0); // Set negative: "Clears the Negative Flag if the Operand is $#00-7F, otherwise sets it."
            else
                set_negative(1);
            if(operand == 0) // Sets the Zero Flag if the Operand is $#00, otherwise clears it.
                set_zero(1);
            else
                set_zero(0);

            reg_ac = (uint8_t) operand;
        }
        case EOR_Z; {
            uint16_t operand = reg_ac ^ memory->mem_read(reg_pc + 1);
            if(operand == #00-7F)
                set_negative(0); // Set negative: "Clears the Negative Flag if the Operand is $#00-7F, otherwise sets it."
            else
                set_negative(1);
            if(operand == 0) // Sets the Zero Flag if the Operand is $#00, otherwise clears it.
                set_zero(1);
            else
                set_zero(0);

            reg_ac = (uint8_t) operand;
        }
        case EOR_ZX; {
            uint16_t operand = reg_ac ^ memory->mem_read(reg_pc + 1);
            if(operand == #00-7F)
                set_negative(0); // Set negative: "Clears the Negative Flag if the Operand is $#00-7F, otherwise sets it."
            else
                set_negative(1);
            if(operand == 0) // Sets the Zero Flag if the Operand is $#00, otherwise clears it.
                set_zero(1);
            else
                set_zero(0);

            reg_ac = (uint8_t) operand;
        }
        case EOR_A; {
            uint16_t operand = reg_ac ^ memory->mem_read(reg_pc + 1);
            if(operand == #00-7F)
                set_negative(0); // Set negative: "Clears the Negative Flag if the Operand is $#00-7F, otherwise sets it."
            else
                set_negative(1);
            if(operand == 0) // Sets the Zero Flag if the Operand is $#00, otherwise clears it.
                set_zero(1);
            else
                set_zero(0);

            reg_ac = (uint8_t) operand;
        }
        case EOR_AX; {
            uint16_t operand = reg_ac ^ memory->mem_read(reg_pc + 1);
            if(operand == #00-7F)
                set_negative(0); // Set negative: "Clears the Negative Flag if the Operand is $#00-7F, otherwise sets it."
            else
                set_negative(1);
            if(operand == 0) // Sets the Zero Flag if the Operand is $#00, otherwise clears it.
                set_zero(1);
            else
                set_zero(0);

            reg_ac = (uint8_t) operand;
        }
        case EOR_AY; {
            uint16_t operand = reg_ac ^ memory->mem_read(reg_pc + 1);
            if(operand == #00-7F)
                set_negative(0); // Set negative: "Clears the Negative Flag if the Operand is $#00-7F, otherwise sets it."
            else
                set_negative(1);
            if(operand == 0) // Sets the Zero Flag if the Operand is $#00, otherwise clears it.
                set_zero(1);
            else
                set_zero(0);

            reg_ac = (uint8_t) operand;
        }
        case EOR_IX; {
            uint16_t operand = reg_ac ^ memory->mem_read(reg_pc + 1);
            if(operand == #00-7F)
                set_negative(0); // Set negative: "Clears the Negative Flag if the Operand is $#00-7F, otherwise sets it."
            else
                set_negative(1);
            if(operand == 0) // Sets the Zero Flag if the Operand is $#00, otherwise clears it.
                set_zero(1);
            else
                set_zero(0);

            reg_ac = (uint8_t) operand;
        }
        case EOR_IY; {
            uint16_t operand = reg_ac ^ memory->mem_read(reg_pc + 1);
            if(operand == #00-7F)
                set_negative(0); // Set negative: "Clears the Negative Flag if the Operand is $#00-7F, otherwise sets it."
            else
                set_negative(1);
            if(operand == 0) // Sets the Zero Flag if the Operand is $#00, otherwise clears it.
                set_zero(1);
            else
                set_zero(0);

            reg_ac = (uint8_t) operand;
        }
        
        /////////
        // LSR //
        /////////

        // Each of the bits in A or M is shift one place to the right.
        // The bit that was in bit 0 is shifted into the carry flag. Bit 7 is set to zero.

        case LSR_AC: {
            set_carry((reg_ac << 7) >> 7);
            reg_ac = reg_ac >> 1;
        }
        case LSR_Z: {
            set_carry((reg_ac << 7) >> 7);
            reg_ac = reg_ac >> 1;
        }
        case LSR_ZX: {
            set_carry((reg_ac << 7) >> 7);
            reg_ac = reg_ac >> 1;
        }
        case LSR_A: {
            set_carry((reg_ac << 7) >> 7);
            //reg_ac = reg_ac << 1;
            memory = memory >> 1;
        }
        case LSR_AX: {
            set_carry((reg_ac << 7) >> 7);
            //reg_ac = reg_ac << 1;
            memory = memory >> 1;
        }

        /////////
        // ORA //
        /////////

        // ORA (Or Memory With Accumulator) performs a logical OR on the operand and the accumulator
        // and stores the result in the accumulator. This opcode is similar in function to AND and EOR.
        
        case ORA_I; {
            uint16_t operand = reg_ac | memory->mem_read(reg_pc + 1);
            if(operand == #00-7F)
                set_negative(0); // Set negative: "Clears the Negative Flag if the Operand is $#00-7F, otherwise sets it."
            else
                set_negative(1);
            if(operand == 0) // Sets the Zero Flag if the Operand is $#00, otherwise clears it.
                set_zero(1);
            else
                set_zero(0);

            reg_ac = (uint8_t) operand;
        }
        case ORA_Z; {
            uint16_t operand = reg_ac | memory->mem_read(reg_pc + 1);
            if(operand == #00-7F)
                set_negative(0); // Set negative: "Clears the Negative Flag if the Operand is $#00-7F, otherwise sets it."
            else
                set_negative(1);
            if(operand == 0) // Sets the Zero Flag if the Operand is $#00, otherwise clears it.
                set_zero(1);
            else
                set_zero(0);

            reg_ac = (uint8_t) operand;
        }
        case ORA_ZX; {
            uint16_t operand = reg_ac | memory->mem_read(reg_pc + 1);
            if(operand == #00-7F)
                set_negative(0); // Set negative: "Clears the Negative Flag if the Operand is $#00-7F, otherwise sets it."
            else
                set_negative(1);
            if(operand == 0) // Sets the Zero Flag if the Operand is $#00, otherwise clears it.
                set_zero(1);
            else
                set_zero(0);

            reg_ac = (uint8_t) operand;
        }
        case ORA_A; {
            uint16_t operand = reg_ac | memory->mem_read(reg_pc + 1);
            if(operand == #00-7F)
                set_negative(0); // Set negative: "Clears the Negative Flag if the Operand is $#00-7F, otherwise sets it."
            else
                set_negative(1);
            if(operand == 0) // Sets the Zero Flag if the Operand is $#00, otherwise clears it.
                set_zero(1);
            else
                set_zero(0);

            reg_ac = (uint8_t) operand;
        }
        case ORA_AX; {
            uint16_t operand = reg_ac | memory->mem_read(reg_pc + 1);
            if(operand == #00-7F)
                set_negative(0); // Set negative: "Clears the Negative Flag if the Operand is $#00-7F, otherwise sets it."
            else
                set_negative(1);
            if(operand == 0) // Sets the Zero Flag if the Operand is $#00, otherwise clears it.
                set_zero(1);
            else
                set_zero(0);

            reg_ac = (uint8_t) operand;
        }
        case ORA_AY; {
            uint16_t operand = reg_ac | memory->mem_read(reg_pc + 1);
            if(operand == #00-7F)
                set_negative(0); // Set negative: "Clears the Negative Flag if the Operand is $#00-7F, otherwise sets it."
            else
                set_negative(1);
            if(operand == 0) // Sets the Zero Flag if the Operand is $#00, otherwise clears it.
                set_zero(1);
            else
                set_zero(0);

            reg_ac = (uint8_t) operand;
        }
        case ORA_IX; {
            uint16_t operand = reg_ac | memory->mem_read(reg_pc + 1);
            if(operand == #00-7F)
                set_negative(0); // Set negative: "Clears the Negative Flag if the Operand is $#00-7F, otherwise sets it."
            else
                set_negative(1);
            if(operand == 0) // Sets the Zero Flag if the Operand is $#00, otherwise clears it.
                set_zero(1);
            else
                set_zero(0);

            reg_ac = (uint8_t) operand;
        }
        case ORA_IY; {
            uint16_t operand = reg_ac | memory->mem_read(reg_pc + 1);
            if(operand == #00-7F)
                set_negative(0); // Set negative: "Clears the Negative Flag if the Operand is $#00-7F, otherwise sets it."
            else
                set_negative(1);
            if(operand == 0) // Sets the Zero Flag if the Operand is $#00, otherwise clears it.
                set_zero(1);
            else
                set_zero(0);

            reg_ac = (uint8_t) operand;
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
        }
        // Change based on memory in the future
        case ROL_Z: {
            uint8_t old_carry = get_carry();
            set_carry(reg_ac >> 7);
            reg_ac = (reg_ac << 1) | old_carry; 
        }
        case ROL_ZX: {
            uint8_t old_carry = get_carry();
            set_carry(reg_ac >> 7);
            reg_ac = (reg_ac << 1) | old_carry; 
        }
        case ROL_A: {
            uint8_t old_carry = get_carry();
            set_carry(reg_ac >> 7);
            reg_ac = (reg_ac << 1) | old_carry; 
        }
        case ROL_AX: {
            uint8_t old_carry = get_carry();
            set_carry(reg_ac >> 7);
            reg_ac = (reg_ac << 1) | old_carry; 
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
            uint8_t old_carry = get_carry();
            set_carry((reg_ac << 7) >> 7);
            reg_ac = (reg_ac >> 1) | (old_carry << 7); 
        }
        case ROR_ZX: {
            uint8_t old_carry = get_carry();
            set_carry((reg_ac << 7) >> 7);
            reg_ac = (reg_ac >> 1) | (old_carry << 7); 
        }
        case ROR_A: {
            uint8_t old_carry = get_carry();
            set_carry((reg_ac << 7) >> 7);
            reg_ac = (reg_ac >> 1) | (old_carry << 7);  
        }
        case ROR_AX: {
            uint8_t old_carry = get_carry();
            set_carry((reg_ac << 7) >> 7);
            reg_ac = (reg_ac >> 1) | (old_carry << 7); 
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
        }

        /////////
        // CLI //
        /////////

        // CLI (Clear Interrupt Disable Flag) clears the Interrupt Flag in the
        // Processor Status Register by setting the 2nd bit 0. To set the interrupt disable flag, use SEI.

        case CLI: {
            set_interrupt((0));
        }

        /////////
        // CLV //
        /////////

        // CLV (Clear Overflow Flag) clears the Overflow Flag in the Processor
        // Status Register by setting the 6th bit 0. There is no opcode for setting the overflow flag.

        case CLV: {
            set_overflow((0));
        }

        /////////
        // CMP //
        /////////

        // Compare sets flags as if a subtraction had been carried out. If the value in the accumulator
        // is equal or greater than the compared value, the Carry will be set. The equal (Z) 
        // and negative (N) flags will be set based on equality or lack thereof and the
        // sign (i.e. A>=$80) of the accumulator.

        case CMP_I: {
            if(reg_ac >= memory){
                set_carry(1);
            }
            if(reg_ac == memory){
                set_zero(1);
            }
            // IF negative set negative bit, but Im not sure how to do that
        }
        case CMP_Z: {
            if(reg_ac >= memory){
                set_carry(1);
            }
            if(reg_ac == memory){
                set_zero(1);
            }
            // IF negative set negative bit, but Im not sure how to do that
        }
        case CMP_ZX: {
            if(reg_ac >= memory){
                set_carry(1);
            }
            if(reg_ac == memory){
                set_zero(1);
            }
            // IF negative set negative bit, but Im not sure how to do that
        }
        case CMP_A: {
            if(reg_ac >= memory){
                set_carry(1);
            }
            if(reg_ac == memory){
                set_zero(1);
            }
            // IF negative set negative bit, but Im not sure how to do that
        }
        case CMP_AX: {
            if(reg_ac >= memory){
                set_carry(1);
            }
            if(reg_ac == memory){
                set_zero(1);
            }
            // IF negative set negative bit, but Im not sure how to do that
        }
        case CMP_AY: {
            if(reg_ac >= memory){
                set_carry(1);
            }
            if(reg_ac == memory){
                set_zero(1);
            }
            // IF negative set negative bit, but Im not sure how to do that
        }
        case CMP_IX: {
            if(reg_ac >= memory){
                set_carry(1);
            }
            if(reg_ac == memory){
                set_zero(1);
            }
            // IF negative set negative bit, but Im not sure how to do that
        }
        case CMP_IY: {
            if(reg_ac >= memory){
                set_carry(1);
            }
            if(reg_ac == memory){
                set_zero(1);
            }
            // IF negative set negative bit, but Im not sure how to do that
        }

        /////////
        // CPX //
        /////////

        // This instruction compares the contents of the X register with another
        // memory held value and sets the zero and carry flags as appropriate.

        case CPX_I: {
            if(reg_ac >= memory){
                set_carry(1);
            }
            if(reg_ac == memory){
                set_zero(1);
            }
            // IF negative set negative bit, but Im not sure how to do that
        }
        case CPX_Z: {
            if(reg_ac >= memory){
                set_carry(1);
            }
            if(reg_ac == memory){
                set_zero(1);
            }
            // IF negative set negative bit, but Im not sure how to do that
        }
        case CPX_A: {
            if(reg_ac >= memory){
                set_carry(1);
            }
            if(reg_ac == memory){
                set_zero(1);
            }
            // IF negative set negative bit, but Im not sure how to do that
        }

        /////////
        // CPY //
        /////////

        // This instruction compares the contents of the Y register with another
        // memory held value and sets the zero and carry flags as appropriate.
        case CPY_I: {
            if(reg_ac >= memory){
                set_carry(1);
            }
            if(reg_ac == memory){
                set_zero(1);
            }
            // IF negative set negative bit, but Im not sure how to do that
        }
        case CPY_Z: {
            if(reg_ac >= memory){
                set_carry(1);
            }
            if(reg_ac == memory){
                set_zero(1);
            }
            // IF negative set negative bit, but Im not sure how to do that
        }
        case CPY_A: {
            if(reg_ac >= memory){
                set_carry(1);
            }
            if(reg_ac == memory){
                set_zero(1);
            }
            // IF negative set negative bit, but Im not sure how to do that
        }
        /////////
        // SEC //
        /////////

        // SEC (Set Carry Flag) sets the Carry Flag in the Processor Status Register 
        // by setting the 0th bit 1. To clear the carry flag, use CLC.

        case SEC: {
            set_carry((1));
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
        }

        /////////
        // SEI //
        /////////

        // SEI (Set Interrupt Disable Flag) sets the Interrupt Flag in the Processor Status Register
        // by setting the 2nd bit 1. To clear the interrupt disable flag, use CLI.

        case SEI: {
            set_interrupt((1));
        }
    }
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