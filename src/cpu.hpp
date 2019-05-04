#ifndef cpu_hpp
#define cpu_hpp

#include <cstdint>
#include <iostream>
#include <vector>
#include <memory>

#include "mem.hpp"

#define ADC_I   0x69
#define ADC_Z   0x65
#define ADC_ZX  0x75
#define ADC_A   0x6D
#define ADC_AX  0x7D
#define ADC_AY  0x79
#define ADC_IX  0x61
#define ADC_IY  0x71

#define AND_I   0x29
#define AND_Z   0x25
#define AND_ZX  0x35
#define AND_A   0x2D
#define AND_AX  0x3D
#define AND_AY  0x39
#define AND_IX  0x21
#define AND_IY  0x31

#define ASL_AC  0x0A
#define ASL_Z   0x06
#define ASL_ZX  0x16
#define ASL_A   0x0E
#define ASL_AX  0x1E

#define BCC     0x90
#define BCS     0xB0
#define BEQ     0xF0
#define BIT_Z   0x24
#define BIT_A   0x2C
#define BMI     0x30
#define BNE     0xD0
#define BPL     0x10
#define BRK     0x00
#define BVC     0x50
#define BVS     0x70

#define CLC     0x18
#define CLD     0xD8
#define CLI     0x58
#define CLV     0xB8

#define CMP_I   0xC9
#define CMP_Z   0xC5
#define CMP_ZX  0xD5
#define CMP_A   0xCD
#define CMP_AX  0xDD
#define CMP_AY  0xD9
#define CMP_IX  0xC1
#define CMP_IY  0xD1

#define CPX_I   0xE0
#define CPX_Z   0xE4
#define CPX_A   0xEC
#define CPY_I   0xC0
#define CPY_Z   0xC4
#define CPY_A   0xCC

#define DEC_Z   0xC6
#define DEC_ZX  0xD6
#define DEC_A   0xCE
#define DEC_AX  0xDE

#define DEX     0xCA
#define DEY     0x88

#define EOR_I   0x49
#define EOR_Z   0x45
#define EOR_ZX  0x55
#define EOR_A   0x4D
#define EOR_AX  0x5D
#define EOR_AY  0x59
#define EOR_IX  0x41
#define EOR_IY  0x51

#define INC_Z   0xE6
#define INC_ZX  0xF6
#define INC_A   0xEE
#define INC_AX  0xFE

#define INX     0xE8
#define INY     0xC8

#define JMP_I   0x6C
#define JMP_A   0x4C

#define JSR     0x20

#define LDA_I   0xA9
#define LDA_Z   0xA5
#define LDA_ZX  0xB5
#define LDA_A   0xAD
#define LDA_AX  0xBD
#define LDA_AY  0xB9
#define LDA_IX  0xA1
#define LDA_IY  0xB1

#define LDX_Z   0xA6
#define LDX_ZY  0xB6
#define LDX_A   0xAE
#define LDX_AY  0xBE
#define LDX_I   0xA2

#define LDY_I   0xA0
#define LDY_Z   0xA4
#define LDY_ZX  0xB4
#define LDY_A   0xAC
#define LDY_AX  0xBC

#define LSR_AC  0x4A
#define LSR_Z   0x46
#define LSR_ZX  0x56
#define LSR_A   0x4E
#define LSR_AX  0x5E

#define NOP     0xEA
#define NOP_2   0xC2

#define ORA_I   0x09
#define ORA_Z   0x05
#define ORA_ZX  0x15
#define ORA_A   0x0D
#define ORA_AX  0x1D
#define ORA_AY  0x19
#define ORA_IX  0x01
#define ORA_IY  0x11

#define PHA     0x48
#define PHP     0x08
#define PLA     0x68
#define PLP     0x28

#define ROL_AC  0x2A
#define ROL_Z   0x26
#define ROL_ZX  0x36
#define ROL_A   0x2E
#define ROL_AX  0x3E

#define ROR_AC  0x6A
#define ROR_Z   0x66
#define ROR_ZX  0x76
#define ROR_A   0x6E
#define ROR_AX  0x7E

#define RTI     0x40
#define RTS     0x60

#define SBC_I   0xE9
#define SBC_Z   0xE5
#define SBC_ZX  0xF5
#define SBC_A   0xED
#define SBC_AX  0xFD
#define SBC_AY  0xF9
#define SBC_IX  0xE1
#define SBC_IY  0xF1

#define SEC     0x38
#define SED     0xF8
#define SEI     0x78

#define STA_Z   0x85
#define STA_ZX  0x95
#define STA_A   0x8D
#define STA_AX  0x9D
#define STA_AY  0x99
#define STA_IX  0x81
#define STA_IY  0x91

#define STX_Z   0x86
#define STX_ZY  0x96
#define STX_A   0x8E

#define STY_Z   0x84
#define STY_ZX  0x94
#define STY_A   0x8C

#define TAX     0xAA
#define TAY     0xA8
#define TSX     0xBA
#define TXA     0x8A
#define TXS     0x9A
#define TYA     0x98

// illegal opcodes

#define ERROR   0xFFFF

#define NEGATIVE(operand) (operand & 0x80)
#define ZERO(operand) (operand == 0)
#define PAGE_SHIFT(new, old) page_shift(new, old)

class Mem;

class CPU {
private:
    std::shared_ptr<Mem> memory;
    
    // register info
    // https://wiki.nesdev.com/w/index.php/CPU_registers
    
    // accumulator
    uint8_t reg_ac;
    
    // indexes
    uint8_t reg_x;
    uint8_t reg_y;
    
    // pc
    uint16_t reg_pc;
    
    // sp
    uint8_t reg_s;
    
    // status register
    uint8_t reg_p;
    
    // addressing modes
    uint8_t imm();
    uint8_t zp();
    uint8_t zp_x();
    uint8_t zp_y();
    uint8_t abs();
    uint8_t abs_x();
    uint8_t abs_y();
    uint8_t ind_x();
    uint8_t ind_y();
    
    // instructions
    void lda(uint8_t operand);
    void ldx(uint8_t operand);
    void ldy(uint8_t operand);
    void sbc(uint8_t operand);
    void adc(uint8_t operand);
    void check_nz(uint8_t operand);
    
    void page_shift(uint16_t shift, uint16_t addr);
    
    void set_negative(bool value);
    void set_overflow(bool value);
    
    void set_break(bool value);
    void set_decimal(bool value);
    void set_interrupt(bool value);
    void set_zero(bool value);
    void set_carry(bool value);
    
    // cycle stuff
    uint8_t cycles;
    
    // clocked events
    uint8_t mem_read(uint64_t index);
    uint16_t mem_read2(uint64_t index);
    void mem_write(uint64_t index, uint8_t value);
    
    uint8_t pc_read();
    uint16_t pc_read2();
    void push(uint8_t value);
    void push16(uint16_t value);
    uint8_t pop();
    uint16_t pop16();
    
public:
    CPU(std::shared_ptr<Mem> memory);
    uint16_t execute();
    
    uint8_t get_ac();
    uint8_t get_x();
    uint8_t get_y();
    uint16_t get_pc();
    uint8_t get_s();
    
    bool get_negative();
    bool get_overflow();
    bool get_break();
    bool get_decimal();
    bool get_interrupt();
    bool get_zero();
    bool get_carry();
};

#endif