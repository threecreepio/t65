#include "nes.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

uint16_t NESCPU_OPS[0x100] = {
/* 00: */ AM_IMP | IN_BRK,
/* 01: */ AM_INX | IN_ORA,
/* 02: */ AM_NON | IX_HLT,
/* 03: */ AM_INX | IX_SLO,
/* 04: */ AM_ZPG | IX_NOP,
/* 05: */ AM_ZPG | IN_ORA,
/* 06: */ AM_ZPG | IN_ASL,
/* 07: */ AM_ZPG | IX_SLO,
/* 08: */ AM_IMP | IN_PHP,
/* 09: */ AM_IMM | IN_ORA,
/* 0a: */ AM_IMP | IN_ASLA,
/* 0b: */ AM_IMM | IX_AAC,
/* 0c: */ AM_ABS | IX_NOP,
/* 0d: */ AM_ABS | IN_ORA,
/* 0e: */ AM_ABS | IN_ASL,
/* 0f: */ AM_ABS | IX_SLO,
/* 10: */ AM_REL | IN_BPL,
/* 11: */ AM_INY | IN_ORA,
/* 12: */ AM_NON | IX_HLT,
/* 13: */ AM_INYW | IX_SLO,
/* 14: */ AM_ZPX | IX_NOP,
/* 15: */ AM_ZPX | IN_ORA,
/* 16: */ AM_ZPX | IN_ASL,
/* 17: */ AM_ZPX | IX_SLO,
/* 18: */ AM_IMP | IN_CLC,
/* 19: */ AM_ABY | IN_ORA,
/* 1a: */ AM_IMP | IX_NOP2,
/* 1b: */ AM_ABYW | IX_SLO,
/* 1c: */ AM_ABX | IX_NOP,
/* 1d: */ AM_ABX | IN_ORA,
/* 1e: */ AM_ABXW | IN_ASL,
/* 1f: */ AM_ABXW | IX_SLO,
/* 20: */ AM_NON | IN_JSR,
/* 21: */ AM_INX | IN_AND,
/* 22: */ AM_NON | IX_HLT,
/* 23: */ AM_INX | IX_RLA,
/* 24: */ AM_ZPG | IN_BIT,
/* 25: */ AM_ZPG | IN_AND,
/* 26: */ AM_ZPG | IN_ROL,
/* 27: */ AM_ZPG | IX_RLA,
/* 28: */ AM_IMP | IN_PLP,
/* 29: */ AM_IMM | IN_AND,
/* 2a: */ AM_IMP | IN_ROLA,
/* 2b: */ AM_IMM | IX_AAC,
/* 2c: */ AM_ABS | IN_BIT,
/* 2d: */ AM_ABS | IN_AND,
/* 2e: */ AM_ABS | IN_ROL,
/* 2f: */ AM_ABS | IX_RLA,
/* 30: */ AM_REL | IN_BMI,
/* 31: */ AM_INY | IN_AND,
/* 32: */ AM_NON | IX_HLT,
/* 33: */ AM_INYW | IX_RLA,
/* 34: */ AM_ZPX | IX_NOP,
/* 35: */ AM_ZPX | IN_AND,
/* 36: */ AM_ZPX | IN_ROL,
/* 37: */ AM_ZPX | IX_RLA,
/* 38: */ AM_IMP | IN_SEC,
/* 39: */ AM_ABY | IN_AND,
/* 3a: */ AM_IMP | IX_NOP2,
/* 3b: */ AM_ABYW | IX_RLA,
/* 3c: */ AM_ABX | IX_NOP,
/* 3d: */ AM_ABX | IN_AND,
/* 3e: */ AM_ABXW | IN_ROL,
/* 3f: */ AM_ABXW | IX_RLA,
/* 40: */ AM_IMP | IN_RTI,
/* 41: */ AM_INX | IN_EOR,
/* 42: */ AM_NON | IX_HLT,
/* 43: */ AM_INX | IX_SRE,
/* 44: */ AM_ZPG | IX_NOP,
/* 45: */ AM_ZPG | IN_EOR,
/* 46: */ AM_ZPG | IN_LSR,
/* 47: */ AM_ZPG | IX_SRE,
/* 48: */ AM_IMP | IN_PHA,
/* 49: */ AM_IMM | IN_EOR,
/* 4a: */ AM_IMP | IN_LSRA,
/* 4b: */ AM_IMM | IX_ASR,
/* 4c: */ AM_ABS | IN_JMP,
/* 4d: */ AM_ABS | IN_EOR,
/* 4e: */ AM_ABS | IN_LSR,
/* 4f: */ AM_ABS | IX_SRE,
/* 50: */ AM_REL | IN_BVC,
/* 51: */ AM_INY | IN_EOR,
/* 52: */ AM_NON | IX_HLT,
/* 53: */ AM_INYW | IX_SRE,
/* 54: */ AM_ZPX | IX_NOP,
/* 55: */ AM_ZPX | IN_EOR,
/* 56: */ AM_ZPX | IN_LSR,
/* 57: */ AM_ZPX | IX_SRE,
/* 58: */ AM_IMP | IN_CLI,
/* 59: */ AM_ABY | IN_EOR,
/* 5a: */ AM_IMP | IX_NOP2,
/* 5b: */ AM_ABYW | IX_SRE,
/* 5c: */ AM_ABX | IX_NOP,
/* 5d: */ AM_ABX | IN_EOR,
/* 5e: */ AM_ABXW | IN_LSR,
/* 5f: */ AM_ABXW | IX_SRE,
/* 60: */ AM_IMP | IN_RTS,
/* 61: */ AM_INX | IN_ADC,
/* 62: */ AM_NON | IX_HLT,
/* 63: */ AM_INX | IX_RRA,
/* 64: */ AM_ZPG | IX_NOP,
/* 65: */ AM_ZPG | IN_ADC,
/* 66: */ AM_ZPG | IN_ROR,
/* 67: */ AM_ZPG | IX_RRA,
/* 68: */ AM_IMP | IN_PLA,
/* 69: */ AM_IMM | IN_ADC,
/* 6a: */ AM_IMP | IN_RORA,
/* 6b: */ AM_IMM | IX_ARR,
/* 6c: */ AM_ABS | IN_JMPI,
/* 6d: */ AM_ABS | IN_ADC,
/* 6e: */ AM_ABS | IN_ROR,
/* 6f: */ AM_ABS | IX_RRA,
/* 70: */ AM_REL | IN_BVS,
/* 71: */ AM_INY | IN_ADC,
/* 72: */ AM_NON | IX_HLT,
/* 73: */ AM_INYW | IX_RRA,
/* 74: */ AM_ZPX | IX_NOP,
/* 75: */ AM_ZPX | IN_ADC,
/* 76: */ AM_ZPX | IN_ROR,
/* 77: */ AM_ZPX | IX_RRA,
/* 78: */ AM_IMP | IN_SEI,
/* 79: */ AM_ABY | IN_ADC,
/* 7a: */ AM_IMP | IX_NOP2,
/* 7b: */ AM_ABYW | IX_RRA,
/* 7c: */ AM_ABX | IX_NOP,
/* 7d: */ AM_ABX | IN_ADC,
/* 7e: */ AM_ABXW | IN_ROR,
/* 7f: */ AM_ABXW | IX_RRA,
/* 80: */ AM_IMM | IX_NOP,
/* 81: */ AM_INX | IN_STA,
/* 82: */ AM_IMM | IX_NOP,
/* 83: */ AM_INX | IX_SAX,
/* 84: */ AM_ZPG | IN_STY,
/* 85: */ AM_ZPG | IN_STA,
/* 86: */ AM_ZPG | IN_STX,
/* 87: */ AM_ZPG | IX_SAX,
/* 88: */ AM_IMP | IN_DEY,
/* 89: */ AM_IMM | IX_NOP,
/* 8a: */ AM_IMP | IN_TXA,
/* 8b: */ AM_IMM | IX_UNK,
/* 8c: */ AM_ABS | IN_STY,
/* 8d: */ AM_ABS | IN_STA,
/* 8e: */ AM_ABS | IN_STX,
/* 8f: */ AM_ABS | IX_SAX,
/* 90: */ AM_REL | IN_BCC,
/* 91: */ AM_INYW | IN_STA,
/* 92: */ AM_NON | IX_HLT,
/* 93: */ AM_INY | IX_UNK,
/* 94: */ AM_ZPX | IN_STY,
/* 95: */ AM_ZPX | IN_STA,
/* 96: */ AM_ZPY | IN_STX,
/* 97: */ AM_ZPY | IX_SAX,
/* 98: */ AM_IMP | IN_TYA,
/* 99: */ AM_ABYW | IN_STA,
/* 9a: */ AM_IMP | IN_TXS,
/* 9b: */ AM_ABY | IX_UNK,
/* 9c: */ AM_ABXW | IX_NOP,
/* 9d: */ AM_ABXW | IN_STA,
/* 9e: */ AM_ABYW | IX_NOP,
/* 9f: */ AM_ABY | IX_UNK,
/* a0: */ AM_IMM | IN_LDY,
/* a1: */ AM_INX | IN_LDA,
/* a2: */ AM_IMM | IN_LDX,
/* a3: */ AM_INX | IX_LAX,
/* a4: */ AM_ZPG | IN_LDY,
/* a5: */ AM_ZPG | IN_LDA,
/* a6: */ AM_ZPG | IN_LDX,
/* a7: */ AM_ZPG | IX_LAX,
/* a8: */ AM_IMP | IN_TAY,
/* a9: */ AM_IMM | IN_LDA,
/* aa: */ AM_IMP | IN_TAX,
/* ab: */ AM_IMM | IX_ATX,
/* ac: */ AM_ABS | IN_LDY,
/* ad: */ AM_ABS | IN_LDA,
/* ae: */ AM_ABS | IN_LDX,
/* af: */ AM_ABS | IX_LAX,
/* b0: */ AM_REL | IN_BCS,
/* b1: */ AM_INY | IN_LDA,
/* b2: */ AM_NON | IX_HLT,
/* b3: */ AM_INY | IX_LAX,
/* b4: */ AM_ZPX | IN_LDY,
/* b5: */ AM_ZPX | IN_LDA,
/* b6: */ AM_ZPY | IN_LDX,
/* b7: */ AM_ZPY | IX_LAX,
/* b8: */ AM_IMP | IN_CLV,
/* b9: */ AM_ABY | IN_LDA,
/* ba: */ AM_IMP | IN_TSX,
/* bb: */ AM_ABY | IX_UNK,
/* bc: */ AM_ABX | IN_LDY,
/* bd: */ AM_ABX | IN_LDA,
/* be: */ AM_ABY | IN_LDX,
/* bf: */ AM_ABY | IX_LAX,
/* c0: */ AM_IMM | IN_CPY,
/* c1: */ AM_INX | IN_CMP,
/* c2: */ AM_IMM | IX_NOP,
/* c3: */ AM_INX | IX_DCP,
/* c4: */ AM_ZPG | IN_CPY,
/* c5: */ AM_ZPG | IN_CMP,
/* c6: */ AM_ZPG | IN_DEC,
/* c7: */ AM_ZPG | IX_DCP,
/* c8: */ AM_IMP | IN_INY,
/* c9: */ AM_IMM | IN_CMP,
/* ca: */ AM_IMP | IN_DEX,
/* cb: */ AM_IMM | IX_AXS,
/* cc: */ AM_ABS | IN_CPY,
/* cd: */ AM_ABS | IN_CMP,
/* ce: */ AM_ABS | IN_DEC,
/* cf: */ AM_ABS | IX_DCP,
/* d0: */ AM_REL | IN_BNE,
/* d1: */ AM_INY | IN_CMP,
/* d2: */ AM_NON | IX_HLT,
/* d3: */ AM_INYW | IX_DCP,
/* d4: */ AM_ZPX | IX_NOP,
/* d5: */ AM_ZPX | IN_CMP,
/* d6: */ AM_ZPX | IN_DEC,
/* d7: */ AM_ZPX | IX_DCP,
/* d8: */ AM_IMP | IN_CLD,
/* d9: */ AM_ABY | IN_CMP,
/* da: */ AM_IMP | IX_NOP2,
/* db: */ AM_ABYW | IX_DCP,
/* dc: */ AM_ABX | IX_NOP,
/* dd: */ AM_ABX | IN_CMP,
/* de: */ AM_ABXW | IN_DEC,
/* df: */ AM_ABXW | IX_DCP,
/* e0: */ AM_IMM | IN_CPX,
/* e1: */ AM_INX | IN_SBC,
/* e2: */ AM_IMM | IX_NOP,
/* e3: */ AM_INX | IX_ISB,
/* e4: */ AM_ZPG | IN_CPX,
/* e5: */ AM_ZPG | IN_SBC,
/* e6: */ AM_ZPG | IN_INC,
/* e7: */ AM_ZPG | IX_ISB,
/* e8: */ AM_IMP | IN_INX,
/* e9: */ AM_IMM | IN_SBC,
/* ea: */ AM_IMP | IN_NOP,
/* eb: */ AM_IMM | IN_SBC,
/* ec: */ AM_ABS | IN_CPX,
/* ed: */ AM_ABS | IN_SBC,
/* ee: */ AM_ABS | IN_INC,
/* ef: */ AM_ABS | IX_ISB,
/* f0: */ AM_REL | IN_BEQ,
/* f1: */ AM_INY | IN_SBC,
/* f2: */ AM_NON | IX_HLT,
/* f3: */ AM_INYW | IX_ISB,
/* f4: */ AM_ZPX | IX_NOP,
/* f5: */ AM_ZPX | IN_SBC,
/* f6: */ AM_ZPX | IN_INC,
/* f7: */ AM_ZPX | IX_ISB,
/* f8: */ AM_IMP | IN_SED,
/* f9: */ AM_ABY | IN_SBC,
/* fa: */ AM_IMP | IX_NOP2,
/* fb: */ AM_ABYW | IX_ISB,
/* fc: */ AM_ABX | IX_NOP,
/* fd: */ AM_ABX | IN_SBC,
/* fe: */ AM_ABXW | IN_INC,
/* ff: */ AM_ABXW | IX_ISB
};

void nescpu_cycle(void) {
    nes.cpu.lastnmi = nes.cpu.wantnmi;
    nes.cpu.lastirq = nes.cpu.wantirq && !nes.cpu.ps.bits.i;
    if (nes.mapper.cpucycle) {
        nes.mapper.cpucycle();
    }
    nes.cpu.cycles += 1;
    nesppu_runcycle();
    nesapu_runcycle();
}


inline void nescpu_writemem(uint16_t addr, uint8_t value) {
    nescpu_cycle();
    switch (addr >> 12) {
        case 0x0:
        case 0x1:
            nes.cpu.ram[addr & 0x7FF] = value;
            break;
        case 0x2:
        case 0x3:
            nesppu_writemem(addr - 0x2000, value);
            break;
        case 0x4:
        case 0x5:
        case 0x6:
        case 0x7:
            nesapu_writemem(addr, value);
            break;
    }
    nes.mapper.writemem(addr, value);
}

uint8_t nescpu_readmemval(uint16_t addr) {
    uint8_t rval = 0xFF;
    uint8_t bank = addr >> 12;

    switch (bank) {
        case 0x0:
        case 0x1:
            rval = nes.cpu.ram[addr & 0x7FF];
            break;
        case 0x2:
        case 0x3:
            rval = nesppu_readmem(addr - 0x2000);
            break;
        case 0x4:
        case 0x5:
        case 0x6:
        case 0x7:
            rval = nesapu_readmem(addr);
            break;
        case 0x8:
        case 0x9:
        case 0xA:
        case 0xB:
        case 0xC:
        case 0xD:
        case 0xE:
        case 0xF:
            rval = nes.cpu.bankdata[bank][addr & 0xFFF];
            break;
    }
    return rval;
}
inline uint8_t nescpu_readmem(uint16_t addr) {
    if (nes.cpu.dma) {
        if ((nes.cpu.dma & (DMA_PCM | DMA_SPR)) == DMA_PCM) {
            bool readingController = (addr == 0x4016) || (addr == 0x4017);
            nes.cpu.dma &= ~DMA_PCM;
            nescpu_readmem(addr);
            
            if (readingController) {
                nescpu_cycle();
            } else {
                nescpu_readmem(addr);
            }

            if (!(nes.apu.clock & 1)) {
                if (readingController) {
                    nescpu_cycle();
                } else {
                    nescpu_readmem(addr);
                }
            }

            nesapu_dpcmfetch();
        }
        if (nes.cpu.dma & DMA_SPR) {
            bool wasPCM = (nes.cpu.dma & DMA_PCM);
            nes.cpu.dma = 0;
            nescpu_readmem(addr);
            wasPCM |= (nes.cpu.dma & DMA_PCM);
            nes.cpu.dma = 0;
            if (!(nes.apu.clock & 1)) {
                nescpu_readmem(addr);
            }
            for (int i = 0; i < 0x100; i++) {
                if (wasPCM) {
                    nesapu_dpcmfetch();
                    nescpu_readmem(addr);
                    wasPCM = 0;
                } else {
                    // check for PCM DMA and schedule it for the next loop
                    wasPCM = (nes.cpu.dma & DMA_PCM);
                    nes.cpu.dma = 0;
                }
                uint8_t val = nescpu_readmem((nes.cpu.dmapage << 8) | i);
                nescpu_writemem(0x2004, val);
            }
            if (wasPCM) {
                nesapu_dpcmfetch();
                nescpu_readmem(addr);
            }
        }
    }

    nescpu_cycle();
    return nescpu_readmemval(addr);
}

void nescpu_donmi(void) {
    nes.cpu.dbg_nmicounter += 1;
    nescpu_readmem(nes.cpu.pc.full);
    nescpu_readmem(nes.cpu.pc.full);
    nescpu_writemem(0x100 | nes.cpu.rsp --, nes.cpu.pc.value.hi);
    nescpu_writemem(0x100 | nes.cpu.rsp --, nes.cpu.pc.value.lo);
    nescpu_writemem(0x100 | nes.cpu.rsp --, nes.cpu.ps.field);
    nes.cpu.ps.bits.i = 1;
    nes.cpu.pc.value.lo = nescpu_readmem(0xFFFA);
    nes.cpu.pc.value.hi = nescpu_readmem(0xFFFB);
}

void nescpu_doirq(void) {
    nescpu_readmem(nes.cpu.pc.full);
    nescpu_readmem(nes.cpu.pc.full);
    nescpu_writemem(0x100 | nes.cpu.rsp --, nes.cpu.pc.value.hi);
    nescpu_writemem(0x100 | nes.cpu.rsp --, nes.cpu.pc.value.lo);
    nescpu_writemem(0x100 | nes.cpu.rsp --, nes.cpu.ps.field);
    nes.cpu.ps.bits.i = 1;
    if (nes.cpu.wantnmi) {
        nes.cpu.wantnmi = false;
        nes.cpu.pc.value.lo = nescpu_readmem(0xFFFA);
        nes.cpu.pc.value.hi = nescpu_readmem(0xFFFB);
    } else {
        nes.cpu.pc.value.lo = nescpu_readmem(0xFFFE);
        nes.cpu.pc.value.hi = nescpu_readmem(0xFFFF);
    }
}


union splitreg run_addressingmode(uint16_t am) {
    union splitreg am_value;
    am_value.full = 0;
    switch (am) {
        // NONE
        case AM_NON: break;

        // IMPLIED
        case AM_IMP:
            nescpu_readmem(nes.cpu.pc.full);
            break;

        // IMMEDIATE
        case AM_IMM:
            {
                am_value.full = nes.cpu.pc.full ++;
            }
            break;

        case AM_REL:
            {
                am_value.value.lo = nescpu_readmem(nes.cpu.pc.full ++);
            }
            break;

        // ZERO PAGE
        case AM_ZPG:
            am_value.value.lo = nescpu_readmem(nes.cpu.pc.full ++);
            break;

        // ZERO PAGE, X
        case AM_ZPX:
            am_value.value.lo = nescpu_readmem(nes.cpu.pc.full ++);
            nescpu_readmem(am_value.full);
            am_value.value.lo += nes.cpu.rx;
            break;

        // ZERO PAGE, Y
        case AM_ZPY:
            am_value.full = nescpu_readmem(nes.cpu.pc.full ++);
            nescpu_readmem(am_value.full);
            am_value.value.lo += nes.cpu.ry;
            break;

        // ABSOLUTE
        case AM_ABS:
            am_value.value.lo = nescpu_readmem(nes.cpu.pc.full ++);
            am_value.value.hi = nescpu_readmem(nes.cpu.pc.full ++);
            break;

        // ABSOLUTE, X
        case AM_ABX:
            {
                am_value.value.lo = nescpu_readmem(nes.cpu.pc.full ++);
                am_value.value.hi = nescpu_readmem(nes.cpu.pc.full ++);
                bool carry = ((int) am_value.value.lo + nes.cpu.rx) >= 0x100;
                am_value.value.lo += nes.cpu.rx;
                if (carry) {
                    nescpu_readmem(am_value.full);
                    am_value.value.hi += 1;
                }
            }
            break;

        // ABSOLUTE, Y
        case AM_ABY:
            {
                am_value.value.lo = nescpu_readmem(nes.cpu.pc.full ++);
                am_value.value.hi = nescpu_readmem(nes.cpu.pc.full ++);
                bool carry = ((int) am_value.value.lo + nes.cpu.ry) >= 0x100;
                am_value.value.lo += nes.cpu.ry;
                if (carry) {
                    nescpu_readmem(am_value.full);
                    am_value.value.hi += 1;
                }
            }
            break;

        case AM_ABXW:
            {
                am_value.value.lo = nescpu_readmem(nes.cpu.pc.full ++);
                am_value.value.hi = nescpu_readmem(nes.cpu.pc.full ++);
                bool carry = ((int) am_value.value.lo + nes.cpu.rx) >= 0x100;
                am_value.value.lo += nes.cpu.rx;
                nescpu_readmem(am_value.full);
                if (carry) {
                    am_value.value.hi += 1;
                }
            }
            break;

        case AM_ABYW:
            {
                am_value.value.lo = nescpu_readmem(nes.cpu.pc.full ++);
                am_value.value.hi = nescpu_readmem(nes.cpu.pc.full ++);
                bool carry = ((int) am_value.value.lo + nes.cpu.ry) >= 0x100;
                am_value.value.lo += nes.cpu.ry;
                nescpu_readmem(am_value.full);
                if (carry) {
                    am_value.value.hi += 1;
                }
            }
            break;


        // INDIRECT, X
        case AM_INX:
            {
                union splitreg tmp;
                tmp.value.hi = 0;
                tmp.value.lo = nescpu_readmem(nes.cpu.pc.full ++);
                nescpu_readmem(tmp.full);
                tmp.value.lo += nes.cpu.rx;
                am_value.value.lo = nescpu_readmem(tmp.full);
                tmp.value.lo += 1;
                am_value.value.hi = nescpu_readmem(tmp.full);
                am_value.full = am_value.full;
            }
            break;

        // INDIRECT, Y
        case AM_INY:
            {
                union splitreg tmp;
                tmp.value.hi = 0;
                tmp.value.lo = nescpu_readmem(nes.cpu.pc.full ++);
                am_value.value.lo = nescpu_readmem(tmp.full);
                tmp.value.lo += 1;
                am_value.value.hi = nescpu_readmem(tmp.full);
                bool carry = ((int)am_value.value.lo + nes.cpu.ry) >= 0x100;
                am_value.value.lo += nes.cpu.ry;
                if (carry) {
                    nescpu_readmem(am_value.full);
                    am_value.value.hi += 1;
                }
            }
            break;

        case AM_INYW:
            {
                union splitreg tmp;
                tmp.value.hi = 0;
                tmp.value.lo = nescpu_readmem(nes.cpu.pc.full ++);
                am_value.value.lo = nescpu_readmem(tmp.full);
                tmp.value.lo += 1;
                am_value.value.hi = nescpu_readmem(tmp.full);
                bool carry = ((int)am_value.value.lo + nes.cpu.ry) >= 0x100;
                am_value.value.lo += nes.cpu.ry;
                nescpu_readmem(am_value.full);
                if (carry) {
                    am_value.value.hi += 1;
                }
                am_value.full = am_value.full;
            }
            break;

        default:
            // todo - break on invalid am
            break;
    }
    return am_value;
}

void in_branch(union splitreg am_value) {
    nescpu_readmem(nes.cpu.pc.full);
    uint8_t rel = am_value.value.lo;
    bool carry = ((int) nes.cpu.pc.value.lo + rel) >= 0x100;
    nes.cpu.pc.value.lo += rel;
    if (rel & 0b10000000)
    {
        if (!carry)
        {
            nescpu_readmem(nes.cpu.pc.full);
            nes.cpu.pc.value.hi --;
        }
    }
    else
    {
        if (carry)
        {
            nescpu_readmem(nes.cpu.pc.full);
            nes.cpu.pc.value.hi ++;
        }
    }
}

int run_instruction(uint8_t in, union splitreg am_value) {
    int retval = CPUSTEP_OK;
    switch (in) {
        case IN_BRK:
            {
                nescpu_readmem(nes.cpu.pc.full);
                nescpu_writemem(0x100 | nes.cpu.rsp --, nes.cpu.pc.value.hi);
                nescpu_writemem(0x100 | nes.cpu.rsp --, nes.cpu.pc.value.lo);
                nescpu_writemem(0x100 | nes.cpu.rsp --, nes.cpu.ps.field);
                nes.cpu.ps.bits.i = 1;
                if (nes.cpu.lastnmi) {
                    nes.cpu.wantnmi = false;
                    nes.cpu.pc.value.lo = nescpu_readmem(0xFFFA);
                    nes.cpu.pc.value.hi = nescpu_readmem(0xFFFB);
                } else {
                    nes.cpu.pc.value.lo = nescpu_readmem(0xFFFE);
                    nes.cpu.pc.value.hi = nescpu_readmem(0xFFFF);
                }
                nes.cpu.lastnmi = false;
                retval = CPUSTEP_BRK;
            }
            break;

        case IN_ADC:
            {
                uint8_t v = nescpu_readmem(am_value.full);
                uint16_t preva = nes.cpu.ra;
                uint16_t result = preva + v + nes.cpu.ps.bits.c;
                nes.cpu.ra = result & 0xFF;
                nes.cpu.ps.bits.v = !!(~(preva ^ v) & (preva ^ result) & 0x80);
                nes.cpu.ps.bits.c = !!(result & 0x100);
                nes.cpu.ps.bits.z = !nes.cpu.ra;
                nes.cpu.ps.bits.n = nes.cpu.ra & 0b10000000;
            }
            break;


        case IX_ISB:
            {
                uint8_t v = nescpu_readmem(am_value.full);
                nescpu_writemem(am_value.full, v);
                v += 1;
                uint16_t preva = nes.cpu.ra;
                uint16_t result = preva + (~v) + (nes.cpu.ps.bits.c);
                nes.cpu.ra = result & 0xFF;
                nes.cpu.ps.bits.v = !!((preva ^ v) & (preva ^ result) & 0x80);
                nes.cpu.ps.bits.c = !(result & 0x100);
                nes.cpu.ps.bits.z = !(nes.cpu.ra);
                nes.cpu.ps.bits.n = nes.cpu.ra & 0b10000000;
                nescpu_writemem(am_value.full, v);
                retval = CPUSTEP_INVALID;
            }
            break;

        case IX_SBC:
            retval = CPUSTEP_INVALID;
        case IN_SBC:
            {
                uint8_t v = nescpu_readmem(am_value.full);
                uint16_t preva = nes.cpu.ra;
                uint16_t result = preva + (~v) + (nes.cpu.ps.bits.c);
                nes.cpu.ra = result & 0xFF;
                nes.cpu.ps.bits.v = !!((preva ^ v) & (preva ^ result) & 0x80);
                nes.cpu.ps.bits.c = !(result & 0x100);
                nes.cpu.ps.bits.z = !(nes.cpu.ra);
                nes.cpu.ps.bits.n = nes.cpu.ra & 0b10000000;
            }
            break;

        case IN_CPX:
            {
                uint8_t value = nescpu_readmem(am_value.full);
                uint8_t result = nes.cpu.rx - value;
                nes.cpu.ps.bits.c = nes.cpu.rx >= value;
                nes.cpu.ps.bits.z = !result;
                nes.cpu.ps.bits.n = result & 0b10000000;
            }
            break;

        case IN_CPY:
            {
                uint8_t value = nescpu_readmem(am_value.full);
                uint8_t result = nes.cpu.ry - value;
                nes.cpu.ps.bits.c = nes.cpu.ry >= value;
                nes.cpu.ps.bits.z = !result;
                nes.cpu.ps.bits.n = result & 0b10000000;
            }
            break;

        case IN_INC:
            {
                uint8_t result = nescpu_readmem(am_value.full);
                nescpu_writemem(am_value.full, result);
                result += 1;
                nes.cpu.ps.bits.z = !result;
                nes.cpu.ps.bits.n = result & 0b10000000;
                nescpu_writemem(am_value.full, result);
            }
            break;

        case IN_DEC:
            {
                uint8_t result = nescpu_readmem(am_value.full);
                nescpu_writemem(am_value.full, result);
                result -= 1;
                nes.cpu.ps.bits.z = !result;
                nes.cpu.ps.bits.n = result & 0b10000000;
                nescpu_writemem(am_value.full, result);
            }
            break;

        case IN_DEX:
            {
                uint8_t result = (nes.cpu.rx -= 1);
                nes.cpu.ps.bits.z = !result;
                nes.cpu.ps.bits.n = result & 0b10000000;
            }
            break;
            
        case IN_DEY:
            {
                uint8_t result = (nes.cpu.ry -= 1);
                nes.cpu.ps.bits.z = !result;
                nes.cpu.ps.bits.n = result & 0b10000000;
            }
            break;
            
        case IN_INX:
            {
                uint8_t result = (nes.cpu.rx += 1);
                nes.cpu.ps.bits.z = !result;
                nes.cpu.ps.bits.n = result & 0b10000000;
            }
            break;

        case IN_INY:
            {
                uint8_t result = (nes.cpu.ry += 1);
                nes.cpu.ps.bits.z = !result;
                nes.cpu.ps.bits.n = result & 0b10000000;
            }
            break;

        case IN_ASLA:
            {
                nes.cpu.ps.bits.c = nes.cpu.ra & 0b10000000;
                nes.cpu.ra <<= 1;
                nes.cpu.ps.bits.z = !nes.cpu.ra;
                nes.cpu.ps.bits.n = nes.cpu.ra & 0b10000000;
            }
            break;

        case IN_ASL:
            {
                uint8_t result = nescpu_readmem(am_value.full);
                nescpu_writemem(am_value.full, result);
                uint8_t before = result;
                result <<= 1;
                nes.cpu.ps.bits.c = before & 0b10000000;
                nes.cpu.ps.bits.z = !result;
                nes.cpu.ps.bits.n = result & 0b10000000;
                nescpu_writemem(am_value.full, result);
            }
            break;

        case IX_SLO:
            {
                uint8_t result = nescpu_readmem(am_value.full);
                nescpu_writemem(am_value.full, result);
                uint8_t before = result;
                result <<= 1;
                nes.cpu.ra |= result;
                nes.cpu.ps.bits.c = before & 0b10000000;
                nes.cpu.ps.bits.z = !nes.cpu.ra;
                nes.cpu.ps.bits.n = nes.cpu.ra & 0b10000000;
                nescpu_writemem(am_value.full, result);
                retval = CPUSTEP_INVALID;
            }
            break;

        case IN_BIT:
            {
                uint8_t result = nescpu_readmem(am_value.full);
                nes.cpu.ps.bits.v = result & 0b01000000;
                nes.cpu.ps.bits.n = result & 0b10000000;
                nes.cpu.ps.bits.z = !(result & nes.cpu.ra);
            }
            break;

        case IN_LSRA:
            {
                nes.cpu.ps.bits.c = nes.cpu.ra & 0x01;
                nes.cpu.ra >>= 1;
                nes.cpu.ps.bits.z = !nes.cpu.ra;
                nes.cpu.ps.bits.n = 0;
            }
            break;

        case IN_LSR:
            {
                uint8_t result = nescpu_readmem(am_value.full);
                nescpu_writemem(am_value.full, result);
                uint8_t before = result;
                result >>= 1;
                nes.cpu.ps.bits.c = before & 0b00000001;
                nes.cpu.ps.bits.z = !result;
                nes.cpu.ps.bits.n = result & 0b10000000;
                nescpu_writemem(am_value.full, result);
            }
            break;

        case IX_SRE:
            {
                uint8_t result = nescpu_readmem(am_value.full);
                nescpu_writemem(am_value.full, result);
                uint8_t before = result;
                result >>= 1;
                nes.cpu.ra ^= result;
                nes.cpu.ps.bits.c = before & 0b00000001;
                nes.cpu.ps.bits.z = !nes.cpu.ra;
                nes.cpu.ps.bits.n = nes.cpu.ra & 0b10000000;
                nescpu_writemem(am_value.full, result);
                retval = CPUSTEP_INVALID;
            }
            break;

        case IN_ROLA:
            {
                bool carry = nes.cpu.ps.bits.c;
                nes.cpu.ps.bits.c = nes.cpu.ra & 0b10000000;
                nes.cpu.ra = (nes.cpu.ra << 1) | carry;
                nes.cpu.ps.bits.z = !nes.cpu.ra;
                nes.cpu.ps.bits.n = nes.cpu.ra & 0b10000000;
            }
            break;

        case IX_RLA:
            {
                uint8_t result = nescpu_readmem(am_value.full);
                nescpu_writemem(am_value.full, result);
                uint8_t before = result;
                uint8_t carry = nes.cpu.ps.bits.c;
                result = (result << 1) | carry;
                nes.cpu.ra &= result;
                nes.cpu.ps.bits.c = before & 0b10000000;
                nes.cpu.ps.bits.z = !nes.cpu.ra;
                nes.cpu.ps.bits.n = nes.cpu.ra & 0b10000000;
                nescpu_writemem(am_value.full, result);
                retval = CPUSTEP_INVALID;
            }
            break;

        case IX_RRA:
            {
                uint8_t v = nescpu_readmem(am_value.full);
                nescpu_writemem(am_value.full, v);
                uint8_t carry = nes.cpu.ps.bits.c;
                nes.cpu.ps.bits.c = (v & 0b00000001);
                v = (v >> 1) | (carry << 7);
                uint16_t preva = nes.cpu.ra;
                uint16_t result = (int) preva + v + nes.cpu.ps.bits.c;
                nes.cpu.ps.bits.v = !!(~(preva ^ v) & (preva ^ result) & 0x80);
                nes.cpu.ra = result & 0xFF;
                nes.cpu.ps.bits.c = !!(result & 0x100);
                nes.cpu.ps.bits.z = !nes.cpu.ra;
                nes.cpu.ps.bits.n = nes.cpu.ra & 0b10000000;
                nescpu_writemem(am_value.full, v);
                retval = CPUSTEP_INVALID;
            }
            break;

        case IN_ROL:
            {
                uint8_t result = nescpu_readmem(am_value.full);
                nescpu_writemem(am_value.full, result);
                uint8_t before = result;
                uint8_t carry = (nes.cpu.ps.bits.c) ? 1 : 0;
                result = (result << 1) | carry;
                nes.cpu.ps.bits.c = before & 0b10000000;
                nes.cpu.ps.bits.z = !result;
                nes.cpu.ps.bits.n = result & 0b10000000;
                nescpu_writemem(am_value.full, result);
            }
            break;

        case IN_RORA:
            {
                bool carry = nes.cpu.ps.bits.c;
                nes.cpu.ps.bits.c = nes.cpu.ra & 0b00000001;
                nes.cpu.ra = (nes.cpu.ra >> 1) | (carry << 7);
                nes.cpu.ps.bits.z = !nes.cpu.ra;
                nes.cpu.ps.bits.n = !!carry;
            }
            break;

        case IN_ROR:
            {
                uint8_t result = nescpu_readmem(am_value.full);
                nescpu_writemem(am_value.full, result);
                uint8_t before = result;
                uint8_t carry = (nes.cpu.ps.bits.c) ? 1 : 0;
                result = (result >> 1) | (carry << 7);
                nes.cpu.ps.bits.z = !result;
                nes.cpu.ps.bits.c = before & 1;
                nes.cpu.ps.bits.n = !!carry;
                nescpu_writemem(am_value.full, result);
            }
            break;

        case IN_EOR:
            {
                uint8_t result = (nes.cpu.ra ^= nescpu_readmem(am_value.full));
                nes.cpu.ps.bits.z = !result;
                nes.cpu.ps.bits.n = result & 0b10000000;
            }
            break;

        case IN_CMP:
            {
                uint8_t value = nescpu_readmem(am_value.full);
                uint8_t result = nes.cpu.ra - value;
                nes.cpu.ps.bits.c = nes.cpu.ra >= value;
                nes.cpu.ps.bits.z = !result;
                nes.cpu.ps.bits.n = result & 0b10000000;
            }
            break;

        case IX_DCP:
            {
                uint8_t value = nescpu_readmem(am_value.full);
                nescpu_writemem(am_value.full, value);
                value -= 1;
                int result = nes.cpu.ra - value;
                nes.cpu.ps.bits.c = result >= 0;
                nes.cpu.ps.bits.z = result == 0;
                nes.cpu.ps.bits.n = result & 0b10000000;
                nescpu_writemem(am_value.full, value);
                retval = CPUSTEP_INVALID;
            }
            break;

        case IN_AND:
            {
                uint8_t result = (nes.cpu.ra &= nescpu_readmem(am_value.full));
                nes.cpu.ps.bits.z = !result;
                nes.cpu.ps.bits.n = result & 0b10000000;
            }
            break;

        case IN_BCC:
            if (!(nes.cpu.ps.bits.c)) in_branch(am_value);
            break;

        case IN_BCS:
            if ( (nes.cpu.ps.bits.c)) in_branch(am_value);
            break;
        case IN_BEQ:
            if ( (nes.cpu.ps.bits.z)) in_branch(am_value);
            break;
        case IN_BNE:
            if (!(nes.cpu.ps.bits.z)) in_branch(am_value);
            break;
        case IN_BMI:
            if ( (nes.cpu.ps.bits.n)) in_branch(am_value);
            break;
        case IN_BPL:
            if (!(nes.cpu.ps.bits.n)) in_branch(am_value);
            break;
        case IN_BVC:
            if (!(nes.cpu.ps.bits.v)) in_branch(am_value);
            break;
        case IN_BVS:
            if ( (nes.cpu.ps.bits.v)) in_branch(am_value);
            break;

        case IN_CLC: nes.cpu.ps.bits.c = 0; break;
        case IN_CLD:
            nes.cpu.ps.bits.d = 0;
            retval = CPUSTEP_CLD;
            break;
        case IN_CLI: nes.cpu.ps.bits.i = 0; break;
        case IN_CLV: nes.cpu.ps.bits.v = 0; break;

        case IN_SEC: nes.cpu.ps.bits.c = 1; break;
        case IN_SED: nes.cpu.ps.bits.d = 1; break;
        case IN_SEI: nes.cpu.ps.bits.i = 1; break;

        case IN_TAX:
            {
                uint8_t result = (nes.cpu.rx = nes.cpu.ra);
                nes.cpu.ps.bits.z = !result;
                nes.cpu.ps.bits.n = result & 0b10000000;
            }
            break;
        case IN_TAY:
            {
            uint8_t result = (nes.cpu.ry = nes.cpu.ra);
                nes.cpu.ps.bits.z = !result;
                nes.cpu.ps.bits.n = result & 0b10000000;
            }
            break;
        case IN_TSX:
            {
                uint8_t result = (nes.cpu.rx = nes.cpu.rsp);
                nes.cpu.ps.bits.z = !result;
                nes.cpu.ps.bits.n = result & 0b10000000;
            }
            break;
        case IN_TXA:
            {
                uint8_t result = (nes.cpu.ra = nes.cpu.rx);
                nes.cpu.ps.bits.z = !result;
                nes.cpu.ps.bits.n = result & 0b10000000;
            }
            break;
        case IN_TXS:
            nes.cpu.rsp = nes.cpu.rx;
            break;
        case IN_TYA:
            {
                uint8_t result = (nes.cpu.ra = nes.cpu.ry);
                nes.cpu.ps.bits.z = !result;
                nes.cpu.ps.bits.n = result & 0b10000000;
            }
            break;

        case IN_ORA:
            {
                uint8_t result = (nes.cpu.ra |= nescpu_readmem(am_value.full));
                nes.cpu.ps.bits.z = !result;
                nes.cpu.ps.bits.n = result & 0b10000000;
            }
            break;

        case IN_LDA:
            {
                uint8_t result = (nes.cpu.ra = nescpu_readmem(am_value.full));
                nes.cpu.ps.bits.z = !result;
                nes.cpu.ps.bits.n = result & 0b10000000;
            }
            break;

        case IN_LDY:
            {
                uint8_t result = (nes.cpu.ry = nescpu_readmem(am_value.full));
                nes.cpu.ps.bits.z = !result;
                nes.cpu.ps.bits.n = result & 0b10000000;
            }
            break;

        case IN_LDX:
            {
                uint8_t result = (nes.cpu.rx = nescpu_readmem(am_value.full));
                nes.cpu.ps.bits.z = !result;
                nes.cpu.ps.bits.n = result & 0b10000000;
            }
            break;
        
        case IX_AXS:
            {
                int result = (nes.cpu.ra & nes.cpu.rx) - nescpu_readmem(am_value.full);
                nes.cpu.ps.bits.c = (result >= 0);
                nes.cpu.ps.bits.z = (result == 0);
                nes.cpu.rx = result & 0xFF;
                nes.cpu.ps.bits.n = result & 0b10000000;
                retval = CPUSTEP_INVALID;
            }
            break;

        case IX_ATX:
        case IX_LAX:
            {
                uint8_t result = (nes.cpu.rx = nes.cpu.ra = nescpu_readmem(am_value.full));
                nes.cpu.ps.bits.z = !result;
                nes.cpu.ps.bits.n = result & 0b10000000;
                retval = CPUSTEP_INVALID;
            }
            break;

        case IN_STA:
            nescpu_writemem(am_value.full, nes.cpu.ra);
            break;

        case IN_STX:
            nescpu_writemem(am_value.full, nes.cpu.rx);
            break;

        case IN_STY:
            nescpu_writemem(am_value.full, nes.cpu.ry);
            break;

        case IN_JMP:
            nes.cpu.pc.full = am_value.full;
            break;
        
        case IN_JMPI:
            {
                nes.cpu.pc.value.lo = nescpu_readmem(am_value.full);
                am_value.value.lo += 1;
                nes.cpu.pc.value.hi = nescpu_readmem(am_value.full);
            }
            break;

        case IN_JSR:
            {
                uint8_t v = nescpu_readmem(nes.cpu.pc.full ++);
                nescpu_readmem(0x100 | nes.cpu.rsp);
                nescpu_writemem(0x100 | nes.cpu.rsp --, nes.cpu.pc.value.hi);
                nescpu_writemem(0x100 | nes.cpu.rsp --, nes.cpu.pc.value.lo);
                nes.cpu.pc.value.hi = nescpu_readmem(nes.cpu.pc.full);
                nes.cpu.pc.value.lo = v;
            }
            break;

        case IN_RTS:
            nescpu_readmem(0x100 | nes.cpu.rsp);
            nes.cpu.pc.value.lo = nescpu_readmem(0x100 | ++ nes.cpu.rsp);
            nes.cpu.pc.value.hi = nescpu_readmem(0x100 | ++ nes.cpu.rsp);
            nescpu_readmem(nes.cpu.pc.full ++);
            break;

        case IN_RTI:
            nescpu_readmem(0x100 | nes.cpu.rsp);
            nes.cpu.ps.field = nescpu_readmem(0x100 | ++ nes.cpu.rsp);
            nes.cpu.pc.value.lo = nescpu_readmem(0x100 | ++ nes.cpu.rsp);
            nes.cpu.pc.value.hi = nescpu_readmem(0x100 | ++ nes.cpu.rsp);
            break;

        case IN_PHA:
            nescpu_writemem(0x100 | nes.cpu.rsp --, nes.cpu.ra);
            break;

        case IN_PHP:
            nescpu_writemem(0x100 | nes.cpu.rsp --, nes.cpu.ps.field | 0x10);
            break;

        case IN_PLA:
            {
                nescpu_readmem(0x100 | nes.cpu.rsp);
                uint8_t result = (nes.cpu.ra = nescpu_readmem(0x100 | ++ nes.cpu.rsp));
                nes.cpu.ps.bits.z = !result;
                nes.cpu.ps.bits.n = result & 0b10000000;
            }
            break;

        case IN_PLP:
            {
                nescpu_readmem(0x100 | nes.cpu.rsp);
                nes.cpu.ps.field = nescpu_readmem(0x100 | ++ nes.cpu.rsp);
            }
            break;


        case IX_SAX:
            {
                nescpu_writemem(am_value.full, nes.cpu.ra & nes.cpu.rx);
                retval = CPUSTEP_INVALID;
            }
            break;

        case IX_AAC:
            {
                nes.cpu.ra &= nescpu_readmem(am_value.full);
                nes.cpu.ps.bits.z = !nes.cpu.ra;
                nes.cpu.ps.bits.c = nes.cpu.ps.bits.n = nes.cpu.ra & 0b10000000;
                retval = CPUSTEP_INVALID;
            }
            break;

        case IX_ARR:
            {
                nes.cpu.ra = ((nes.cpu.ra & nescpu_readmem(am_value.full) >> 1) | (nes.cpu.ps.bits.c << 7));
                uint8_t result = nes.cpu.ra;
                nes.cpu.ps.bits.z = !result;
                nes.cpu.ps.bits.n = result & 0b10000000;
                nes.cpu.ps.bits.c = result & 0b01000000;
                nes.cpu.ps.bits.v = nes.cpu.ps.bits.c ^ (result & 0b00100000);
                retval = CPUSTEP_INVALID;
            }
            break;

        case IX_ASR:
            {
                uint8_t result = (nes.cpu.ra &= nescpu_readmem(am_value.full));
                nes.cpu.ps.bits.c = (result & 0x01);
                result = (nes.cpu.ra >>= 1);
                nes.cpu.ps.bits.z = !result;
                nes.cpu.ps.bits.n = 0;
                retval = CPUSTEP_INVALID;
            }
            break;

        case IX_NOP:
            nescpu_readmem(am_value.full);
            retval = CPUSTEP_INVALID;
            break;

        case IN_NOP:
            return 0;

        case IX_HLT:
        case IX_UNK:
        case IX_NOP2:
        default:
            retval = CPUSTEP_INVALID;
            break;
    }
    if (nes.cpu.lastnmi) {
        nescpu_donmi();
        nes.cpu.wantnmi = 0;
        return retval | CPUSTEP_NMI;
    }
    if (nes.cpu.lastirq) {
        nescpu_doirq();
    }
    return retval;
}


int nescpu_step(void) {
    uint8_t opcode = nescpu_readmem(nes.cpu.pc.full);
    uint16_t op = NESCPU_OPS[opcode];
    uint16_t am = op & 0b11110000000;
    uint16_t in = op & 0b00001111111;
    nes.cpu.pc.full += 1;
    union splitreg am_value = run_addressingmode(am);
    return run_instruction(in, am_value);
}

void nescpu_initialize(void) {
    nesc_memset(&nes.cpu, 0, sizeof(struct nescpu));
}

void nescpu_poweron(void) {
    nesc_memset(&nes.cpu, 0, sizeof(struct nescpu));
    nes.cpu.pc.full = 0x0000;
    nes.cpu.ps.field = 0x20;
    nes.cpu.ra = 0;
    nes.cpu.rx = 0;
    nes.cpu.ry = 0;
    nes.cpu.rsp = 0;
    nes.cpu.cycles = 0;
}

void nescpu_reset(void) {
    nescpu_readmem(nes.cpu.pc.full);
    nescpu_readmem(nes.cpu.pc.full);
	nescpu_readmem(0x100 | nes.cpu.rsp --);
	nescpu_readmem(0x100 | nes.cpu.rsp --);
	nescpu_readmem(0x100 | nes.cpu.rsp --);
    nes.cpu.ps.bits.i = 1;
	nes.cpu.pc.value.lo = nescpu_readmem(0xFFFC);
	nes.cpu.pc.value.hi = nescpu_readmem(0xFFFD);
}
