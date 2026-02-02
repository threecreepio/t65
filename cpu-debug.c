#include <stdint.h>
#include <stdio.h>
#include "sys.h"

const char InstructionNames[0x100 * 0x4] = "ADC AND ASL ASL BCC BCS BEQ BIT BMI BNE BPL BRK BVC BVS CLC CLD CLI CLV CMP CPX CPY DEC DEX DEY EOR INC INX INY JMP JMP JSR LDA LDX LDY LSR LSR NOP ORA PHA PHP PLA PLP ROL ROL ROR ROR RTI RTS SBC SEC SED SEI STA STX STY TAX TAY TSX TXA TXS TYA AAC ARR ASR ATX AXS DCP HLT ISB LAX NOP NOP RLA RRA SAX SBC SLO SRE UNK ";
void syscpu_printstate() {
        uint32_t cycles = sys.cpu.cycles;
        uint32_t diff = cycles - sys.cpu.cycles2;
        sys.cpu.cycles2 = cycles;
        uint16_t pcbefore = sys.cpu.pc.full;

        uint8_t opcode = syscpu_readmemval(pcbefore);
        uint16_t am = syscpu_ops[opcode] & 0b11110000000;
        uint16_t in = syscpu_ops[opcode] & 0b00001111111;
        
        fprintf(stderr, "\n");
        fprintf(stderr, "$%04X> " ,pcbefore);
        fprintf(stderr, "%02X ", opcode);
        switch (am) {
            // 0 operands
            case AM_IMP:
            case AM_NON:
                fprintf(stderr, "      ");
                break;
            // 1 operand
            case AM_IMM:
            case AM_ZPG:
            case AM_ZPX:
            case AM_ZPY:
            case AM_INX:
            case AM_INY:
            case AM_INXW:
            case AM_INYW:
                fprintf(stderr, "%02X    ", syscpu_readmemval(pcbefore + 1));
                break;
            // 2 operands
            case AM_ABS:
            case AM_ABX:
            case AM_ABY:
            case AM_ABXW:
            case AM_ABYW:
            case AM_REL:
                fprintf(stderr, "%02X %02X ", syscpu_readmemval(pcbefore + 1), syscpu_readmemval(pcbefore + 2));
                break;
        }
        
        const int stackpage = 0x20;
        int stackdepth = (0x100 - sys.cpu.rsp);
        if (stackdepth >= stackpage) {
            fprintf(stderr, ">");
        } else {
            fprintf(stderr, "|");
        }

        for (int i=0; i<stackdepth % stackpage; ++i) {
            fprintf(stderr, " ");
        }

        fprintf(stderr, "%.*s ", 3, &InstructionNames[in * 4]);

        switch (am) {
            case AM_IMM:
                fprintf(stderr, "#$%02X   ", syscpu_readmemval(pcbefore + 1));
                break;
            case AM_INX:
            case AM_INXW:
                fprintf(stderr, "($%02X,X)", syscpu_readmemval(pcbefore + 1));
                break;
            case AM_INY:
            case AM_INYW:
                fprintf(stderr, "($%02X),Y", syscpu_readmemval(pcbefore + 1));
                break;
            case AM_ABS:
                fprintf(stderr, "$%02X%02X  ", syscpu_readmemval(pcbefore + 2), syscpu_readmemval(pcbefore + 1));
                break;
            case AM_ZPX:
                fprintf(stderr, "$%02X,X  ", syscpu_readmemval(pcbefore + 1));
                break;
            case AM_ABX:
            case AM_ABXW:
                fprintf(stderr, "$%02X%02X,X", syscpu_readmemval(pcbefore + 2), syscpu_readmemval(pcbefore + 1));
                break;
            case AM_ZPY:
                fprintf(stderr, "$%02X,Y  ", syscpu_readmemval(pcbefore + 1));
                break;
            case AM_ABY:
            case AM_ABYW:
                fprintf(stderr, "$%02X%02X,Y", syscpu_readmemval(pcbefore + 2), syscpu_readmemval(pcbefore + 1));
                break;
            case AM_REL:
                fprintf(stderr, "$%04X  ", pcbefore + 2 + ((int8_t) syscpu_readmemval(pcbefore + 1)));
                break;
            case AM_ZPG:
                fprintf(stderr, "$%02X    ", syscpu_readmemval(pcbefore + 1));
                break;
            default:
                fprintf(stderr, "       ");
                break;
        }

        for (int i=stackdepth%stackpage; i<stackpage; ++i) {
            fprintf(stderr, " ");
        }

        fprintf(stderr, "A:%02X ", sys.cpu.ra);
        fprintf(stderr, "X:%02X ", sys.cpu.rx);
        fprintf(stderr, "Y:%02X ", sys.cpu.ry);
        fprintf(stderr, "S:%02X ", sys.cpu.rsp);
        fprintf(stderr, "P:%c%c%c%c%c%c ",
            (sys.cpu.ps.bits.i) ? 'I' : '.',
            (sys.cpu.ps.bits.z) ? 'Z' : '.',
            (sys.cpu.ps.bits.n) ? 'N' : '.',
            (sys.cpu.ps.bits.c) ? 'C' : '.',
            (sys.cpu.ps.bits.v) ? 'V' : '.',
            (sys.cpu.ps.bits.d) ? 'D' : '.'
        );

        fprintf(stderr, "CYC:%6d (+%d) ", cycles % 1000000, diff);
}

void syscpu_printmem() {
    fprintf(stderr, "\n\nCurrent RAM State:\n");
    for (int p=0; p<0x8000; p+=0x100) {
        int found = 0;
        for (int c=p; c<p+0x100; ++c) {
            if (sys.cpu.memory[c]) {
                found = 1;
                break;
            }
        }
        if (!found) continue;

        fprintf(stderr, "\n      0  1  2  3  4  5  6  7   8  9  A  B  C  D  E  F\n");
        for (int i=p; i<p+0x100; i+=0x10) {
            found = 0;
            for (int c=i; c<i+0x10; ++c) {
                if (sys.cpu.memory[c]) {
                    found = 1;
                    break;
                }
            }
            if (!found) continue;
            fprintf(stderr, "%03X:", i);
            for (int j=i; j<i+0x10; ++j) {
                if (j % 0x10 == 8) fprintf(stderr, " ");
                fprintf(stderr, " %02X", sys.cpu.memory[j]);
            }
            fprintf(stderr, "\n");
        }
    }
}
