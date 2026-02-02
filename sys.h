#pragma once
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

struct sys;

#define CPUSTEP_OK 0
#define CPUSTEP_RTS 1
#define CPUSTEP_BRK 2
#define CPUSTEP_INVALID 3
#define CPUSTEP_CTRLC 4

extern uint16_t syscpu_ops[0x100];

enum AddressingModes {
    AM_NON  =  0 << 7,
    AM_IMP  =  1 << 7,
    AM_IMM  =  2 << 7,
    AM_INX  =  3 << 7,
    AM_INY  =  4 << 7,
    AM_INXW =  5 << 7,
    AM_INYW =  6 << 7,
    AM_ZPX  =  7 << 7,
    AM_ZPY  =  8 << 7,
    AM_ZPG  =  9 << 7,
    AM_ABS  = 10 << 7,
    AM_ABX  = 11 << 7,
    AM_ABY  = 12 << 7,
    AM_ABXW = 13 << 7,
    AM_ABYW = 14 << 7,
    AM_REL  = 15 << 7,
};

enum InstructionSet {
    IN_ADC,IN_AND,IN_ASL,IN_ASLA,IN_BCC,IN_BCS,IN_BEQ,IN_BIT,IN_BMI,IN_BNE,
    IN_BPL,IN_BRK,IN_BVC,IN_BVS,IN_CLC,IN_CLD,IN_CLI,IN_CLV,IN_CMP,IN_CPX,
    IN_CPY,IN_DEC,IN_DEX,IN_DEY,IN_EOR,IN_INC,IN_INX,IN_INY,IN_JMP,IN_JMPI,
    IN_JSR,IN_LDA,IN_LDX,IN_LDY,IN_LSR,IN_LSRA,IN_NOP,IN_ORA,IN_PHA,IN_PHP,
    IN_PLA,IN_PLP,IN_ROL,IN_ROLA,IN_ROR,IN_RORA,IN_RTI,IN_RTS,IN_SBC,IN_SEC,
    IN_SED,IN_SEI,IN_STA,IN_STX,IN_STY,IN_TAX,IN_TAY,IN_TSX,IN_TXA,IN_TXS,
    IN_TYA,IX_AAC,IX_ARR,IX_ASR,IX_ATX,IX_AXS,IX_DCP,IX_HLT,IX_ISB,IX_LAX,
    IX_NOP,IX_NOP2,IX_RLA,IX_RRA,IX_SAX,IX_SBC,IX_SLO,IX_SRE,IX_UNK,
};

struct syscpu_flagbits {
    bool c  : 1;
    bool z  : 1;
    bool i  : 1;
    bool d  : 1;
    bool b1 : 1;
    bool b2 : 1;
    bool v  : 1;
    bool n  : 1;
};

union syscpu_flags {
    struct syscpu_flagbits bits;
    uint8_t field;
};

typedef struct uint16_hilo {
    uint8_t lo;
    uint8_t hi;
} uint16_hilo_t;

union splitreg {
    uint16_t full;
    uint16_hilo_t value;
};

struct syscpu {
    uint8_t memory[0x8000];
    uint8_t ra, rx, ry, rsp;
    union syscpu_flags ps;
    union splitreg pc;
    uint32_t cycles;
    uint8_t wantirq, lastirq;
};

struct sys {
    struct syscpu cpu;
    uint8_t rom[0x8000];
    void (*writemem)(uint16_t addr, uint8_t value);
    uint8_t (*readmem)(uint16_t addr);
};

void sys_reset(bool hard);
void sys_step(void);

void syscpu_poweron(void);
void syscpu_reset(void);
uint8_t syscpu_readmem(uint16_t addr);
void syscpu_writemem(uint16_t addr, uint8_t value);
void syscpu_printstate(void);
int syscpu_step(void);
uint8_t syscpu_readmemval(uint16_t addr);

extern struct sys sys;
