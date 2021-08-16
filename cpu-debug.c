#include <stdint.h>
#include <stdio.h>
#include "nes.h"

const char InstructionNames[0x100 * 0x4] = "ADC AND ASL ASL BCC BCS BEQ BIT BMI BNE BPL BRK BVC BVS CLC CLD CLI CLV CMP CPX CPY DEC DEX DEY EOR INC INX INY JMP JMP JSR LDA LDX LDY LSR LSR NOP ORA PHA PHP PLA PLP ROL ROL ROR ROR RTI RTS SBC SEC SED SEI STA STX STY TAX TAY TSX TXA TXS TYA AAC ARR ASR ATX AXS DCP HLT ISB LAX NOP NOP RLA RRA SAX SBC SLO SRE UNK ";
void nescpu_printstate() {
        uint32_t ppuclock = nes.ppu.clock;
        uint32_t cycles = nes.cpu.cycles;
        uint16_t pcbefore = nes.cpu.pc.full;

        uint8_t opcode = nescpu_readmemval(pcbefore);
        uint16_t am = NESCPU_OPS[opcode] & 0b11110000000;
        uint16_t in = NESCPU_OPS[opcode] & 0b00001111111;

        printf("$%04X> %.*s ", pcbefore, 3, &InstructionNames[in * 4]);
        switch (am) {
            case AM_IMM:
                printf("#$%02X            ", nescpu_readmemval(pcbefore + 1));
                break;
            case AM_INX:
            case AM_INXW:
                printf("($%02X,X)         ", nescpu_readmemval(pcbefore + 1));
                break;
            case AM_INY:
            case AM_INYW:
                printf("($%02X),Y         ", nescpu_readmemval(pcbefore + 1));
                break;
            case AM_ABS:
                printf("$%02X%02X           ", nescpu_readmemval(pcbefore + 2), nescpu_readmemval(pcbefore + 1));
                break;
            case AM_ZPX:
                printf("$%02X,X           ", nescpu_readmemval(pcbefore + 1));
                break;
            case AM_ABX:
            case AM_ABXW:
                printf("$%02X%02X,X         ", nescpu_readmemval(pcbefore + 2), nescpu_readmemval(pcbefore + 1));
                break;
            case AM_ZPY:
                printf("$%02X,Y           ", nescpu_readmemval(pcbefore + 1));
                break;
            case AM_ABY:
            case AM_ABYW:
                printf("$%02X%02X,Y         ", nescpu_readmemval(pcbefore + 2), nescpu_readmemval(pcbefore + 1));
                break;
            case AM_REL:
                printf("$%04X           ", pcbefore + 2 + ((int8_t) nescpu_readmemval(pcbefore + 1)));
                break;
            case AM_ZPG:
                printf("$%02X             ", nescpu_readmemval(pcbefore + 1));
                break;
            default:
                if (in == IN_JSR) {
                    printf("$%02X%02X           ", nescpu_readmemval(pcbefore + 2), nescpu_readmemval(pcbefore + 1));
                } else {
                    printf("                ");
                }
                break;
        }

        printf("A:%02X ", nes.cpu.ra);
        printf("X:%02X ", nes.cpu.rx);
        printf("Y:%02X ", nes.cpu.ry);
        printf("S:%02X ", nes.cpu.rsp);
        printf("P:%c%c%c%c%c%c ",
            (nes.cpu.ps.bits.i) ? 'I' : ' ',
            (nes.cpu.ps.bits.z) ? 'Z' : ' ',
            (nes.cpu.ps.bits.n) ? 'N' : ' ',
            (nes.cpu.ps.bits.c) ? 'C' : ' ',
            (nes.cpu.ps.bits.v) ? 'V' : ' ',
            (nes.cpu.ps.bits.d) ? 'D' : ' '
        );

        printf("PPU:%03d,%03d", ppuclock, nes.cpu.dbg_nmicounter);
        printf(" CYC:%d\n", cycles);
}


void nescpu_printmem() {

    printf("\nCurrent RAM State:\n");
    for (int p=0; p<0x800; p+=0x100) {
        int found = 0;
        for (int c=p; c<p+0x100; ++c) {
            if (nes.cpu.ram[c]) {
                found = 1;
                break;
            }
        }
        if (!found) continue;

        printf("\n      0  1  2  3  4  5  6  7   8  9  A  B  C  D  E  F\n");
        for (int i=p; i<p+0x100; i+=0x10) {
            found = 0;
            for (int c=i; c<i+0x10; ++c) {
                if (nes.cpu.ram[c]) {
                    found = 1;
                    break;
                }
            }
            if (!found) continue;
            printf("%03X:", i);
            for (int j=i; j<i+0x10; ++j) {
                if (j % 0x10 == 8) printf(" ");
                printf(" %02X", nes.cpu.ram[j]);
            }
            printf("\n");
        }
    }
}
