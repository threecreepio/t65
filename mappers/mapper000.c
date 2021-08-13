#include "../nes.h"

void mapper000_reset() {
    nes.cpu.bankdata[0x8] = nes.prgrom[0];
    nes.cpu.bankdata[0x9] = nes.prgrom[1];
    nes.cpu.bankdata[0xA] = nes.prgrom[2];
    nes.cpu.bankdata[0xB] = nes.prgrom[3];
    nes.cpu.bankdata[0xC] = nes.prgrom[4];
    nes.cpu.bankdata[0xD] = nes.prgrom[5];
    nes.cpu.bankdata[0xE] = nes.prgrom[6];
    nes.cpu.bankdata[0xF] = nes.prgrom[7];

    nes.ppu.bankdata[0x0] = nes.chrrom[0];
    nes.ppu.bankdata[0x1] = nes.chrrom[1];
    nes.ppu.bankdata[0x2] = nes.chrrom[2];
    nes.ppu.bankdata[0x3] = nes.chrrom[3];
    nes.ppu.bankdata[0x4] = nes.chrrom[4];
    nes.ppu.bankdata[0x5] = nes.chrrom[5];
    nes.ppu.bankdata[0x6] = nes.chrrom[6];
    nes.ppu.bankdata[0x7] = nes.chrrom[7];

    for (int i=0x0; i<=0xF; ++i) nes.ppu.bankflags[i] |= 1;
}

void mapper000_reset_v() {
    mapper000_reset();
    nes.ppu.bankdata[0x8] = nes.ppu.vram[0];
    nes.ppu.bankdata[0x9] = nes.ppu.vram[1];
    nes.ppu.bankdata[0xA] = nes.ppu.vram[0];
    nes.ppu.bankdata[0xB] = nes.ppu.vram[1];
    nes.ppu.bankdata[0xC] = nes.ppu.vram[0];
    nes.ppu.bankdata[0xD] = nes.ppu.vram[1];
    nes.ppu.bankdata[0xE] = nes.ppu.vram[0];
    nes.ppu.bankdata[0xF] = nes.ppu.vram[1];
}

void mapper000_reset_h() {
    mapper000_reset();
    nes.ppu.bankdata[0x8] = nes.ppu.vram[0];
    nes.ppu.bankdata[0x9] = nes.ppu.vram[0];
    nes.ppu.bankdata[0xA] = nes.ppu.vram[1];
    nes.ppu.bankdata[0xB] = nes.ppu.vram[1];
    nes.ppu.bankdata[0xC] = nes.ppu.vram[0];
    nes.ppu.bankdata[0xD] = nes.ppu.vram[0];
    nes.ppu.bankdata[0xE] = nes.ppu.vram[1];
    nes.ppu.bankdata[0xF] = nes.ppu.vram[1];
}

void mapper000_writemem(uint16_t addr, uint8_t value) {
    if ((addr & 0xF000) == 0x3000) {
        nes.breaking = 1;
    }
}

void mapper000_initialize() {
    nes.mapper.writemem = mapper000_writemem;
    if (nes.rominfo.ines_flags & 1) {
        nes.mapper.reset = mapper000_reset_v;
    } else {
        nes.mapper.reset = mapper000_reset_h;
    }
}
