#include "nes.h"

void nes_initialize(void) {
    nescpu_initialize();
    nesppu_initialize();
    nesapu_initialize();
}

void nes_reset(bool hard) {
    if (hard) {
        if (nes.mapper.poweron) {
            nes.mapper.poweron();
        }
        nescpu_poweron();
        nesppu_poweron();
        nesapu_poweron();
    }
    if (nes.mapper.reset) {
        nes.mapper.reset();
    }
    nescpu_reset();
    nesppu_reset();
    nesapu_reset();
    nes.cpu.cycles = 0;
}

void nes_step(void) {
    nescpu_printstate();
    nescpu_step();
}

int nes_frameadvance(void) {
    int f = nes.ppu.framesrendered;
    while (f == nes.ppu.framesrendered) {
        nescpu_printstate();
        int err = nescpu_step();
        if (err != 0)  return err;
    }
    return 0;
}