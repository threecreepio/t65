#include "nes.h"
#include <stdio.h>
#include <string.h>

const uint8_t PRECOMP_RCHR[0x100] = {
        0x00,0x80,0x40,0xC0,0x20,0xA0,0x60,0xE0,0x10,0x90,0x50,0xD0,0x30,0xB0,0x70,0xF0,
        0x08,0x88,0x48,0xC8,0x28,0xA8,0x68,0xE8,0x18,0x98,0x58,0xD8,0x38,0xB8,0x78,0xF8,
        0x04,0x84,0x44,0xC4,0x24,0xA4,0x64,0xE4,0x14,0x94,0x54,0xD4,0x34,0xB4,0x74,0xF4,
        0x0C,0x8C,0x4C,0xCC,0x2C,0xAC,0x6C,0xEC,0x1C,0x9C,0x5C,0xDC,0x3C,0xBC,0x7C,0xFC,
        0x02,0x82,0x42,0xC2,0x22,0xA2,0x62,0xE2,0x12,0x92,0x52,0xD2,0x32,0xB2,0x72,0xF2,
        0x0A,0x8A,0x4A,0xCA,0x2A,0xAA,0x6A,0xEA,0x1A,0x9A,0x5A,0xDA,0x3A,0xBA,0x7A,0xFA,
        0x06,0x86,0x46,0xC6,0x26,0xA6,0x66,0xE6,0x16,0x96,0x56,0xD6,0x36,0xB6,0x76,0xF6,
        0x0E,0x8E,0x4E,0xCE,0x2E,0xAE,0x6E,0xEE,0x1E,0x9E,0x5E,0xDE,0x3E,0xBE,0x7E,0xFE,
        0x01,0x81,0x41,0xC1,0x21,0xA1,0x61,0xE1,0x11,0x91,0x51,0xD1,0x31,0xB1,0x71,0xF1,
        0x09,0x89,0x49,0xC9,0x29,0xA9,0x69,0xE9,0x19,0x99,0x59,0xD9,0x39,0xB9,0x79,0xF9,
        0x05,0x85,0x45,0xC5,0x25,0xA5,0x65,0xE5,0x15,0x95,0x55,0xD5,0x35,0xB5,0x75,0xF5,
        0x0D,0x8D,0x4D,0xCD,0x2D,0xAD,0x6D,0xED,0x1D,0x9D,0x5D,0xDD,0x3D,0xBD,0x7D,0xFD,
        0x03,0x83,0x43,0xC3,0x23,0xA3,0x63,0xE3,0x13,0x93,0x53,0xD3,0x33,0xB3,0x73,0xF3,
        0x0B,0x8B,0x4B,0xCB,0x2B,0xAB,0x6B,0xEB,0x1B,0x9B,0x5B,0xDB,0x3B,0xBB,0x7B,0xFB,
        0x07,0x87,0x47,0xC7,0x27,0xA7,0x67,0xE7,0x17,0x97,0x57,0xD7,0x37,0xB7,0x77,0xF7,
        0x0F,0x8F,0x4F,0xCF,0x2F,0xAF,0x6F,0xEF,0x1F,0x9F,0x5F,0xDF,0x3F,0xBF,0x7F,0xFF
};

const uint32_t PRECOMP_CHRLoBit[0x10] = {
        0x00000000,0x00000001,0x00000100,0x00000101,0x00010000,0x00010001,0x00010100,0x00010101,
        0x01000000,0x01000001,0x01000100,0x01000101,0x01010000,0x01010001,0x01010100,0x01010101
};

const uint32_t PRECOMP_CHRHiBit[0x10] = {
        0x00000000,0x00000002,0x00000200,0x00000202,0x00020000,0x00020002,0x00020200,0x00020202,
        0x02000000,0x02000002,0x02000200,0x02000202,0x02020000,0x02020002,0x02020200,0x02020202
};

void incvram_horizontal(void) {
    if ((nes.ppu.vramaddr & 0x1F) == 0x1F) {
        nes.ppu.vramaddr ^= 0x41F;
    } else {
        nes.ppu.vramaddr += 1;
    }
}
void incvram_vertical(void) {
    uint32_t vram = nes.ppu.vramaddr;
    if ((vram & 0x7000) == 0x7000) {
        uint32_t yscroll = vram & 0x3E0;
        vram &= 0xFFF;
        if (yscroll == 0x3A0) {
            vram ^= 0xBA0;
        } else if (yscroll == 0x3E0) {
            vram ^= 0x3E0;
        } else {
            vram += 0x020;
        }
    } else {
        vram += 0x1000;
    }
    nes.ppu.vramaddr = vram;
}

void incvram() {
    if (nes.ppu.reg2000 & 0x04) {
        nes.ppu.vramaddr += 32;
    } else {
        nes.ppu.vramaddr += 1;
    }
    nes.ppu.vramaddr &= 0x7FFF;
}

void nesppu_initialize(void) {
    nesc_memset(&nes.ppu, 0, sizeof(struct nesppu));
    nesc_memset(nes.ppu.palette, 0x3F, sizeof(nes.ppu.palette));
    nesc_memset(nes.ppu.sprite, 0xFF, sizeof(nes.ppu.sprite));
    nesc_memset(nes.ppu.screen, 0x00, sizeof(nes.ppu.screen));
    nes.ppu.enabled = 1;
}

void nesppu_poweron(void) {
    nesc_memset(nes.ppu.palette, 0x3F, sizeof(nes.ppu.palette));
    nesc_memset(nes.ppu.sprite, 0xFF, sizeof(nes.ppu.sprite));
    nesc_memset(nes.ppu.screen, 0x00, sizeof(nes.ppu.screen));
    nes.ppu.grayscale = 0x3F;
    nes.ppu.hvtog = 1;
    nes.ppu.scanlineshort = 1;
    nes.ppu.screenat = 0;

    for (int i=0; i<0x10; ++i) {
        if (!nes.ppu.bankdata[i]) {
            nes.ppu.bankdata[i] = nes.ppu.vram[0];
            nes.ppu.bankflags[i] = 0;
        }
    }
}

void nesppu_reset(void) {
    nes.ppu.spraddr = 0;
    nes.ppu.rendering = 0;
    nes.ppu.onscreen = 0;
    nes.ppu.ioval = 0;
    nes.ppu.iomode = 0;
    nes.ppu.clock = 0;
    nes.ppu.scanline = 0;
    nes.ppu.sprbuff = &nes.ppu.sprite[256];
    nes.ppu.screenat = 0;
    nes.ppu.sprite[0x120] = 0xFF;
}

void nesppu_writemem(uint16_t addr, uint8_t value) {
    nes.ppu.readlatch = value;
    switch (addr & 0x07) {
        case 0: {
            if ((value & 0x80) && !(nes.ppu.reg2000 & 0x80) && (nes.ppu.reg2002 & 0x80)) {
                nes.cpu.wantnmi = 1;
            }
            if (nes.ppu.scanline == 241 && !(value & 0x80) && (nes.ppu.clock < 3)) {
                nes.cpu.wantnmi = 0;
            }
            nes.ppu.reg2000 = value;
            nes.ppu.intreg &= 0x73FF;
            nes.ppu.intreg |= (value & 3) << 10;
            break;
        }
        case 1: {
            nes.ppu.reg2001 = value;
            if ((value & 0x18) && nes.ppu.scanline < 240) {
                nes.ppu.rendering = 1;
            } else {
                nes.ppu.rendering = 0;
            }
            nes.ppu.coloremphasis = (value & 0xE0) << 1;
            nes.ppu.grayscale = (value & 0x01) ? 0x30 : 0x3F;
            break;
        }
        case 2: { break; }
        case 3: {
            nes.ppu.spraddr = value;
            break;
        }
        case 4: {
            if (nes.ppu.rendering) {
                value = 0xFF;
            }
            if ((nes.ppu.spraddr & 0x03) == 0x02) {
                value &= 0xE3;
            }
            nes.ppu.sprite[nes.ppu.spraddr] = value;
            nes.ppu.spraddr = (nes.ppu.spraddr + 1) & 0xFF;
            break;
        }
        case 5: {
            if (nes.ppu.hvtog) {
                nes.ppu.intreg &= 0x7FE0;
                nes.ppu.intreg |= (value & 0xF8) >> 3;
                nes.ppu.intx = (value & 0x7);
            } else {
                nes.ppu.intreg &= 0x0C1F;
                nes.ppu.intreg |= (value & 0x07) << 12;
                nes.ppu.intreg |= (value & 0xF8) << 2;
            }
            nes.ppu.hvtog = !nes.ppu.hvtog;
            break;
        }
        case 6: {
            if (nes.ppu.hvtog) {
                nes.ppu.intreg &= 0x00FF;
                nes.ppu.intreg |= (value & 0x3F) << 8;
            } else {
                nes.ppu.intreg &= 0x7F00;
                nes.ppu.intreg |= value;
                nes.ppu.vramaddr = nes.ppu.intreg;
            }
            nes.ppu.hvtog = !nes.ppu.hvtog;
            break;
        }
        case 7: {
            if ((nes.ppu.vramaddr & 0x3F00) == 0x3F00) {
                uint8_t addr = nes.ppu.vramaddr & 0x1F;
                value &= 0x3F;
                nes.ppu.palette[addr] = value;
                if (!(addr & 0x3)) {
                    nes.ppu.palette[addr ^ 0x10] = value;
                }
                incvram();
            } else {
                nes.ppu.ioval = value;
                nes.ppu.iomode = 6;
            }
            break;
        }
    }
}

uint8_t nesppu_readmem(uint16_t addr) {
    switch (addr & 0x7) {
        case 0: case 1: case 3: case 5: case 6:
            return nes.ppu.readlatch;
            break;

            // 0x2002
        case 2: {
            nes.ppu.hvtog = 1;
            uint8_t tmp = nes.ppu.reg2002 | (nes.ppu.readlatch & 0x1F);
            if (tmp & 0x80) {
                nes.ppu.reg2002 &= 0x60;
            }
            if (nes.ppu.scanline == 241) {
                if (nes.ppu.clock == 0) {
                    tmp &= ~0x80;
                }
                if (nes.ppu.clock < 3) {
                    nes.cpu.wantnmi = 0;
                }
            }
            return nes.ppu.readlatch = tmp;
            break;
        }

            // 0x2004
        case 4: {
            if (nes.ppu.rendering) {
                nes.ppu.readlatch = nes.ppu.sprite[nes.ppu.sprptr];
            } else {
                nes.ppu.readlatch = nes.ppu.sprite[nes.ppu.spraddr];
            }
            return nes.ppu.readlatch;
            break;
        }
        case 7: {
            nes.ppu.iomode = 5;
            if ((nes.ppu.vramaddr & 0x3F00) == 0x3F00) {
                nes.ppu.readlatch &= 0xC0;
                if (nes.ppu.reg2001 & 0x01) {
                    return nes.ppu.readlatch |= nes.ppu.palette[nes.ppu.vramaddr & 0x1F] & 0x30;
                } else {
                    return nes.ppu.readlatch |= nes.ppu.palette[nes.ppu.vramaddr & 0x1F];
                }
            } else {
                return nes.ppu.readlatch = nes.ppu.buf2007;
            }
            break;
        }
    }
    return nes.ppu.readlatch;
}


void nesppu_writechr(uint16_t addr, uint8_t value) {
    if (nes.ppu.bankflags[addr >> 10] & 1) {
        nes.ppu.bankdata[addr >> 10][addr & 0x3FF] = value;
    }
}
uint8_t nesppu_readchr(uint16_t addr) {
    return nes.ppu.bankdata[addr >> 10][addr & 0x3FF];
}

void processsprites(void) {
    if (nes.ppu.clock == 0) {
        // if nes.ppu.sprite address is not zero at the beginning of the pre-render nes.ppu.scanline
        // then copy the contents of its 8-byte page into the first 8 bytes
        if (nes.ppu.scanline == -1 && (nes.ppu.spraddr & 0xF8) != 0) {
            //memcpy(nes.ppu.sprite, nes.ppu.sprite + (nes.ppu.spraddr & 0xF8), 8);
        }
        nes.ppu.sprptr = nes.ppu.spraddr = nes.ppu.spraddrh = 0;
    }
    else if (nes.ppu.clock <= 64) {
        if (nes.ppu.clock & 1) {
            nes.ppu.sprtmp = nes.ppu.sprite[nes.ppu.sprptr = 0x120];
        } else {
            nes.ppu.sprite[nes.ppu.sprptr = 0x100 | nes.ppu.spraddrh] = nes.ppu.sprtmp;
			nes.ppu.spraddrh = (nes.ppu.spraddrh + 1) & 0x1F;
        }
    }
    else if (nes.ppu.clock <= 256) {
        if (nes.ppu.clock == 65) {
            nes.ppu.sprstate = 0;
            nes.ppu.sprzero = 0;
            nes.ppu.sprcount_tmp = 0;
        }
        switch (nes.ppu.sprstate) {
            case 0:	// evaluate current Y coordinate
                if (nes.ppu.clock & 1) {
                    nes.ppu.sprtmp = nes.ppu.sprite[nes.ppu.sprptr = nes.ppu.spraddr];
                } else {
                    nes.ppu.sprite[nes.ppu.sprptr = 0x100 | nes.ppu.spraddrh] = nes.ppu.sprtmp;
                    if ((nes.ppu.scanline >= nes.ppu.sprtmp) && (nes.ppu.scanline <= nes.ppu.sprtmp + ((nes.ppu.reg2000 & 0x20) ? 0xF : 0x7))) {
                        nes.ppu.sprcount_tmp++;
                        // sprite is in range
                        if (nes.ppu.spraddr == 0) {
                            nes.ppu.sprzero = 1;
                        }
                        // start fetching data bytes
                        nes.ppu.sprstate = 1;
                        nes.ppu.spraddr = (nes.ppu.spraddr + 1) & 0xFF;
                        nes.ppu.spraddrh = (nes.ppu.spraddrh + 1) & 0x1F;
                    } else {
                        // checked all 64 sprites? skip to state 4
                        nes.ppu.spraddr = (nes.ppu.spraddr + 4) & 0xFC;
                        // wrapped around? skip to state 4
                        if (!nes.ppu.spraddr) {
                            nes.ppu.sprstate = 4;
                        }
                    }
                }
                break;
            case 1:	// Y-coordinate is in range, copy remaining bytes
                if (nes.ppu.clock & 1) {
                    nes.ppu.sprtmp = nes.ppu.sprite[nes.ppu.sprptr = nes.ppu.spraddr];
                } else {
                    nes.ppu.sprite[nes.ppu.sprptr = 0x100 | nes.ppu.spraddrh] = nes.ppu.sprtmp;
                    nes.ppu.spraddr = (nes.ppu.spraddr + 1) & 0xFF;
                    nes.ppu.spraddrh = (nes.ppu.spraddrh + 1) & 0x1F;
                    if (!(nes.ppu.spraddrh & 0x3)) {
                        if (nes.ppu.spraddr < 0x4) {
                            nes.ppu.sprstate = 4;
                        } else if (nes.ppu.spraddrh == 0) {
                            nes.ppu.sprstate = 2;
                        } else {
                            nes.ppu.sprstate = 0;
                        }
                    }
                }
                break;
            case 2:	// exactly 8 sprites detected, go through 'weird' evaluation
                if (nes.ppu.clock & 1) {
                    nes.ppu.sprtmp = nes.ppu.sprite[nes.ppu.sprptr = nes.ppu.spraddr];
                } else {
                    nes.ppu.sprptr = 0x100 | nes.ppu.spraddrh;	// write suppressed
                    if ((nes.ppu.scanline >= nes.ppu.sprtmp) && (nes.ppu.scanline <= nes.ppu.sprtmp + ((nes.ppu.reg2000 & 0x20) ? 0xF : 0x7))) {
                        // 9th sprite found "in range"
                        nes.ppu.sprstate = 3;
                        nes.ppu.reg2002 |= 0x20;	// set sprite overflow flag
                        nes.ppu.spraddr = (nes.ppu.spraddr + 1) & 0xFF;
                        nes.ppu.spraddrh = 1; // this doesn't actually happen, but it's simpler to track this way
                    } else {
                        if ((nes.ppu.spraddr & 0x3) == 0x3) {
                            nes.ppu.spraddr = (nes.ppu.spraddr + 1) & 0xFF;
                        } else {
                            nes.ppu.spraddr = (nes.ppu.spraddr + 5) & 0xFF;
                        }
                        if (nes.ppu.spraddr < 0x4) {
                            nes.ppu.sprstate = 4;
                        }
                    }
                }
                break;
            case 3:	// 9th sprite detected, fetch next 3 bytes
                if (nes.ppu.clock & 1) {
                    nes.ppu.sprtmp = nes.ppu.sprite[nes.ppu.sprptr = nes.ppu.spraddr];
                } else {
                    nes.ppu.sprptr = 0x100;	// write suppressed
                    nes.ppu.spraddr = (nes.ppu.spraddr + 1) & 0xFF;
                    nes.ppu.spraddrh++;
                    // fetched 4 bytes
                    if (nes.ppu.spraddrh == 4) {
                        // if we're misaligned, go back to the beginning of the sprite
                        if (nes.ppu.spraddr & 0x3) {
                            nes.ppu.spraddr = (nes.ppu.spraddr & 0xFC);
                        } else {
                            nes.ppu.spraddr = (nes.ppu.spraddr + 4) & 0xFC;
                        }
                        nes.ppu.spraddrh = 0;
                        nes.ppu.sprstate = 4;
                    }
                }
                break;
            case 4:	// no more sprites to evaluate, thrash until HBLANK
                if (nes.ppu.clock & 1) {
                    nes.ppu.sprtmp = nes.ppu.sprite[nes.ppu.sprptr = nes.ppu.spraddr];
                } else {
                    nes.ppu.sprptr = 0x100 | nes.ppu.spraddrh;
                    nes.ppu.spraddr = (nes.ppu.spraddr + 4) & 0xFC;
                }
                break;
        }
    } else if (nes.ppu.clock <= 320) {
        if (nes.ppu.clock == 257) {
            nes.ppu.sprcount = nes.ppu.sprcount_tmp * 4;
            nes.ppu.spr0line = nes.ppu.sprzero;
            nes.ppu.spraddr = nes.ppu.spraddrh = 0;
        }
        nes.ppu.sprptr = 0x100 | nes.ppu.spraddrh;
        if (((nes.ppu.clock - 1) & 4) < 4) {
            nes.ppu.spraddrh = (nes.ppu.spraddrh + 1) & 0x1F;
        }
    } else {
        nes.ppu.sprptr = 0x100;
    }
}

void nesppu_run_renderpixel(void) {
    int32_t tc = 0;
    int32_t palindex = 0;
    if ((nes.ppu.reg2001 & 0b00001000) && ((nes.ppu.clock >= 8) || (nes.ppu.reg2001 & 0b00000010))) {
        tc = nes.ppu.tiledata[nes.ppu.clock + nes.ppu.intx];
    }
    if ((nes.ppu.reg2001 & 0b00010000) && ((nes.ppu.clock >= 8) || (nes.ppu.reg2001 & 0b00000100))) {
        for (int sp = 0; sp < nes.ppu.sprcount; sp += 4) {
            int spritepx = nes.ppu.clock - nes.ppu.sprdata[sp >> 2][9];
            if (spritepx & ~7) continue;
            uint8_t sprdata = nes.ppu.sprdata[sp >> 2][spritepx];
            if (!(sprdata & 0x3)) continue;

            if (nes.ppu.spr0line && (sp == 0) && (tc & 0x3) && (nes.ppu.clock < 255)) {
                nes.ppu.reg2002 |= 0x40;
                nes.ppu.spr0line = false;
            }
            if (!((tc & 0x3) && (nes.ppu.sprdata[sp >> 2][8] & 0x20))) {
                tc = sprdata | 0x10;
            }
            break;
        }
    }
    if (tc & 0x3) {
        palindex = nes.ppu.palette[tc & 0x1F];
    } else {
        palindex = nes.ppu.palette[0];
    }
    if (!nes.ppu.rendering && ((nes.ppu.vramaddr & 0x3F00) == 0x3F00)) {
        palindex = nes.ppu.palette[nes.ppu.vramaddr & 0x1F];
    }
    palindex &= nes.ppu.grayscale;
    palindex |= nes.ppu.coloremphasis;

    nes.ppu.screen[nes.ppu.screenat] = palindex;
    nes.ppu.screenat ++;
}

void nesppu_run_endscanline(void) {
    nes.ppu.clock = 0;
    nes.ppu.scanline = (nes.ppu.scanline + 1);
    if (nes.ppu.scanline < 240) {
        /* render */
        nes.ppu.onscreen = true;
    } else if (nes.ppu.scanline == 240) {
        nes.ppu.rendering = nes.ppu.onscreen = false;
        nes.ppu.screenat = 0;
        nes.ppu.framesrendered += 1;
    } else if (nes.ppu.scanline == 241) {
        nes.ppu.reg2002 |= 0x80;
        if (nes.ppu.reg2000 & 0x80) {
            nes.cpu.wantnmi = 1;
        }
    } else if (nes.ppu.scanline == 261) {
        nes.ppu.scanline = nes.ppu.scanline = -1;
        nes.ppu.scanlineshort = !nes.ppu.scanlineshort;
        if (nes.ppu.reg2001 & 0x18) {
            nes.ppu.rendering = true;
        }
    }
}

void nesppu_run_background(void) {
    switch (nes.ppu.clock) {
        case   0:	case   8:	case  16:	case  24:	case  32:	case  40:	case  48:	case  56:
        case  64:	case  72:	case  80:	case  88:	case  96:	case 104:	case 112:	case 120:
        case 128:	case 136:	case 144:	case 152:	case 160:	case 168:	case 176:	case 184:
        case 192:	case 200:	case 208:	case 216:	case 224:	case 232:	case 240:	case 248:
        case 320:	case 328:
            nes.ppu.renderaddr = 0x2000 | (nes.ppu.vramaddr & 0xFFF);
            break;
        case   1:	case   9:	case  17:	case  25:	case  33:	case  41:	case  49:	case  57:
        case  65:	case  73:	case  81:	case  89:	case  97:	case 105:	case 113:	case 121:
        case 129:	case 137:	case 145:	case 153:	case 161:	case 169:	case 177:	case 185:
        case 193:	case 201:	case 209:	case 217:	case 225:	case 233:	case 241:	case 249:
        case 321:	case 329:
            nes.ppu.patternaddr = (nes.ppu.renderdata[0] << 4) | (nes.ppu.vramaddr >> 12) | ((nes.ppu.reg2000 & 0b00010000) << 8);
            break;
        case   2:	case  10:	case  18:	case  26:	case  34:	case  42:	case  50:	case  58:
        case  66:	case  74:	case  82:	case  90:	case  98:	case 106:	case 114:	case 122:
        case 130:	case 138:	case 146:	case 154:	case 162:	case 170:	case 178:	case 186:
        case 194:	case 202:	case 210:	case 218:	case 226:	case 234:	case 242:	case 250:
        case 322:	case 330:
            nes.ppu.renderaddr = 0x23C0 | (nes.ppu.vramaddr & 0xC00) | ((nes.ppu.vramaddr & 0x380) >> 4) | ((nes.ppu.vramaddr & 0x1C) >> 2);
            break;
        case   3:	case  11:	case  19:	case  27:	case  35:	case  43:	case  51:	case  59:
        case  67:	case  75:	case  83:	case  91:	case  99:	case 107:	case 115:	case 123:
        case 131:	case 139:	case 147:	case 155:	case 163:	case 171:	case 179:	case 187:
        case 195:	case 203:	case 211:	case 219:	case 227:	case 235:	case 243:	case 251: {
            uint32_t *curtiledata = (uint32_t*) &nes.ppu.tiledata[nes.ppu.clock + 13];
            uint32_t tl = ((nes.ppu.renderdata[1] >> (((nes.ppu.vramaddr & 0x40) >> 4) | (nes.ppu.vramaddr & 0x2))) & 3) * 0x04040404;
            curtiledata[0] = tl;
            curtiledata[1] = tl;
            break;
        }
        case 323:	case 331: {
            uint32_t *curtiledata = (uint32_t*) &nes.ppu.tiledata[nes.ppu.clock - 323];
            uint32_t tl = ((nes.ppu.renderdata[1] >> (((nes.ppu.vramaddr & 0x40) >> 4) | (nes.ppu.vramaddr & 0x2))) & 3) * 0x04040404;
            curtiledata[0] = tl;
            curtiledata[1] = tl;
            break;
        }
        case   4:	case  12:	case  20:	case  28:	case  36:	case  44:	case  52:	case  60:
        case  68:	case  76:	case  84:	case  92:	case 100:	case 108:	case 116:	case 124:
        case 132:	case 140:	case 148:	case 156:	case 164:	case 172:	case 180:	case 188:
        case 196:	case 204:	case 212:	case 220:	case 228:	case 236:	case 244:	case 252:
        case 324:	case 332:
            nes.ppu.renderaddr = nes.ppu.patternaddr;
            break;
        case   5:	case  13:	case  21:	case  29:	case  37:	case  45:	case  53:	case  61:
        case  69:	case  77:	case  85:	case  93:	case 101:	case 109:	case 117:	case 125:
        case 133:	case 141:	case 149:	case 157:	case 165:	case 173:	case 181:	case 189:
        case 197:	case 205:	case 213:	case 221:	case 229:	case 237:	case 245:	case 253: {
            uint8_t tc = PRECOMP_RCHR[nes.ppu.renderdata[2]];
            uint32_t *curtiledata = (uint32_t*) &nes.ppu.tiledata[nes.ppu.clock + 11];
            curtiledata[0] |= PRECOMP_CHRLoBit[tc & 0xF];
            curtiledata[1] |= PRECOMP_CHRLoBit[tc >> 4];
            break;
        }
        case 325:	case 333: {
            uint8_t tc = PRECOMP_RCHR[nes.ppu.renderdata[2]];
            uint32_t *curtiledata = (uint32_t*) &nes.ppu.tiledata[nes.ppu.clock - 325];
            curtiledata[0] |= PRECOMP_CHRLoBit[tc & 0xF];
            curtiledata[1] |= PRECOMP_CHRLoBit[tc >> 4];
        }
            break;
        case   6:	case  14:	case  22:	case  30:	case  38:	case  46:	case  54:	case  62:
        case  70:	case  78:	case  86:	case  94:	case 102:	case 110:	case 118:	case 126:
        case 134:	case 142:	case 150:	case 158:	case 166:	case 174:	case 182:	case 190:
        case 198:	case 206:	case 214:	case 222:	case 230:	case 238:	case 246:	case 254:
        case 326:	case 334:
            nes.ppu.renderaddr = nes.ppu.patternaddr | 8;
            break;
        case   7:	case  15:	case  23:	case  31:	case  39:	case  47:	case  55:	case  63:
        case  71:	case  79:	case  87:	case  95:	case 103:	case 111:	case 119:	case 127:
        case 135:	case 143:	case 151:	case 159:	case 167:	case 175:	case 183:	case 191:
        case 199:	case 207:	case 215:	case 223:	case 231:	case 239:	case 247:	case 255: {
            uint8_t tc = PRECOMP_RCHR[nes.ppu.renderdata[3]];
            uint32_t *curtiledata = (uint32_t *) &nes.ppu.tiledata[nes.ppu.clock + 9];
            curtiledata[0] |= PRECOMP_CHRHiBit[tc & 0xF];
            curtiledata[1] |= PRECOMP_CHRHiBit[tc >> 4];
            incvram_horizontal();
            if (nes.ppu.clock == 255) {
                incvram_vertical();
            }
            break;
        }
        case 327:	case 335: {
            uint8_t tc = PRECOMP_RCHR[nes.ppu.renderdata[3]];
            uint32_t *curtiledata = (uint32_t *) &nes.ppu.tiledata[nes.ppu.clock - 327];
            curtiledata[0] |= PRECOMP_CHRHiBit[tc & 0xF];
            curtiledata[1] |= PRECOMP_CHRHiBit[tc >> 4];
            incvram_horizontal();
            break;
        }
    }
}

void nesppu_run_sprites(void) {
    switch (nes.ppu.clock) {
        case 256:
            nes.ppu.vramaddr &= ~0x41F;
            nes.ppu.vramaddr |= nes.ppu.intreg & 0x41F;
            case 264:	case 272:	case 280:	case 288:	case 296:	case 304:	case 312:
            nes.ppu.renderaddr = 0x2000 | (nes.ppu.vramaddr & 0xFFF);
            break;
        case 257:	case 265:	case 273:	case 281:	case 289:	case 297:	case 305:	case 313:
            break;
        case 258:	case 266:	case 274:	case 282:	case 290:	case 298:	case 306:	case 314:
            nes.ppu.renderaddr = 0x2000 | (nes.ppu.vramaddr & 0xFFF);
            break;
        case 259:	case 267:	case 275:	case 283:	case 291:	case 299:	case 307:	case 315: {
            uint32_t sprite = (nes.ppu.clock >> 1) & 0x1C;
            uint8_t *sprbuff = nes.ppu.sprbuff;
            uint8_t tc = sprbuff[sprite | 1];
            uint8_t scanlinespr = nes.ppu.scanline - sprbuff[sprite];
            if (nes.ppu.reg2000 & 0x20) {
                nes.ppu.patternaddr = ((tc & 0xFE) << 4)
                                   | ((tc & 0x01) << 12)
                                   | ((scanlinespr & 7) ^ ((sprbuff[sprite | 2] & 0x80) ? 0x17 : 0x00) ^ ((scanlinespr & 0x8) << 1));
            } else {
                nes.ppu.patternaddr = (tc << 4)
                                   | ((scanlinespr & 7) ^ ((sprbuff[sprite | 2] & 0x80) ? 0x7 : 0x0))
                                   | ((nes.ppu.reg2000 & 0x08) << 9);
            }
            break;
        }
        case 260:	case 268:	case 276:	case 284:	case 292:	case 300:	case 308:	case 316:
            nes.ppu.renderaddr = nes.ppu.patternaddr;
            break;
        case 261:	case 269:	case 277:	case 285:	case 293:	case 301:	case 309:	case 317: {
            uint32_t sprite = (nes.ppu.clock >> 1) & 0x1E;
            uint8_t *sprbuff = nes.ppu.sprbuff;
            uint8_t tc = 0;
            if (sprbuff[sprite] & 0x40) {
                tc = nes.ppu.renderdata[2];
            } else {
                tc = PRECOMP_RCHR[nes.ppu.renderdata[2]];
            }
            uint32_t tl = (sprbuff[sprite] & 0x3) * 0x04040404;
            uint32_t *curtiledata = (uint32_t *) &(nes.ppu.sprdata[sprite >> 2]);
            curtiledata[0] = PRECOMP_CHRLoBit[tc & 0xF] | tl;
            curtiledata[1] = PRECOMP_CHRLoBit[tc >> 4] | tl;
            break;
        }
        case 262:	case 270:	case 278:	case 286:	case 294:	case 302:	case 310:	case 318:
            nes.ppu.renderaddr = nes.ppu.patternaddr | 8;
            break;
        case 263:	case 271:	case 279:	case 287:	case 295:	case 303:	case 311:	case 319: {
            uint32_t sprite = (nes.ppu.clock >> 1) & 0x1E;
            uint8_t *sprbuff = nes.ppu.sprbuff;
            uint8_t tc = 0;
            if (sprbuff[sprite] & 0x40) {
                tc = nes.ppu.renderdata[3];
            } else {
                tc = PRECOMP_RCHR[nes.ppu.renderdata[3]];
            }
            uint32_t *curtiledata = (uint32_t *) &nes.ppu.sprdata[sprite >> 2];
            curtiledata[0] |= PRECOMP_CHRHiBit[tc & 0xF];
            curtiledata[1] |= PRECOMP_CHRHiBit[tc >> 4];
            ((uint8_t *)curtiledata)[8] = sprbuff[sprite];
            ((uint8_t *)curtiledata)[9] = sprbuff[sprite | 1];
            break;
        }
        case 336:	case 338:
            nes.ppu.renderaddr = 0x2000 | (nes.ppu.vramaddr & 0xFFF);
        case 337:	case 339:
        case 340:
            break;
    }
}

void nesppu_runcycle(void) {
    for (int i=0; i<3; ++i) {
        nes.ppu.clock += 1;
        if (nes.ppu.clock == 256) {
            if (nes.ppu.scanline < 240) {
                //nesc_memset(nes.ppu.tiledata, 0, sizeof(nes.ppu.tiledata));
            }
        } else if (nes.ppu.clock >= 279 && nes.ppu.clock <= 303) {
            if (nes.ppu.scanline == -1 && nes.ppu.rendering) {
                nes.ppu.vramaddr &= ~0x7BE0;
                nes.ppu.vramaddr |= nes.ppu.intreg & 0x7BE0;
            }
        } else if (nes.ppu.clock == 338) {
            nes.ppu.scanlineend = 341;
            if (nes.ppu.scanline == -1 && nes.ppu.rendering && nes.ppu.scanlineshort) {
                nes.ppu.scanlineend -= 1;
            }
        } else if (nes.ppu.clock == nes.ppu.scanlineend) {
            nesppu_run_endscanline();
        } else if (nes.ppu.clock == 1 && nes.ppu.scanline == -1) {
            nes.ppu.reg2002 &= 0x80;
        }

        if (nes.ppu.rendering) {
            if (nes.ppu.enabled) {
                processsprites();
            }
            if (nes.ppu.clock & 1) {
                if (nes.ppu.iomode) {
                    nes.ppu.renderdata[(nes.ppu.clock >> 1) & 3] = 0;
                    if (nes.ppu.iomode == 2) {
                        nes.ppu.ioval = 0;
                        nesppu_writechr(nes.ppu.renderaddr, 0);
                    }
                } else {
                    nes.ppu.renderdata[(nes.ppu.clock >> 1) & 3] = nesppu_readchr(nes.ppu.renderaddr);
                }
            }
            if (nes.ppu.enabled) {
                nesppu_run_background();
                nesppu_run_sprites();
            }
            if (!(nes.ppu.clock & 1)) {
                if (nes.mapper.ppucycle) {
                    nes.mapper.ppucycle();
                }
                if (nes.ppu.iomode == 2) {
                    nesppu_writechr(nes.ppu.renderaddr, nes.ppu.renderaddr & 0xFF);
                }
            }
        }
        if (nes.ppu.iomode) {
            uint16_t vram = nes.ppu.vramaddr & 0x3FFF;
            if (nes.ppu.iomode >= 5 && !nes.ppu.rendering) {
                if (nes.mapper.ppucycle) {
                    nes.mapper.ppucycle();
                }
            } else if (nes.ppu.iomode == 2) {
                if (!nes.ppu.rendering) {
                    nesppu_writechr(vram, nes.ppu.ioval);
                }
            } else if (nes.ppu.iomode == 1) {
                nes.ppu.iomode += 1;
                if (!nes.ppu.rendering) {
                    nes.ppu.buf2007 = nesppu_readchr(vram);
                }
            }
            nes.ppu.iomode -= 2;
            if (!nes.ppu.iomode) {
                if (nes.ppu.rendering) {
                    if (nes.ppu.clock != 255) {
                        incvram_vertical();
                    }
                    if (!(((nes.ppu.clock & 7) == 7) && !(255 <= nes.ppu.clock && nes.ppu.clock <= 319))) {
                        incvram_horizontal();
                    }
                } else {
                    incvram();
                }
            }
        }
        if (!nes.ppu.rendering && !nes.ppu.iomode) {
            if (nes.mapper.ppucycle) {
                nes.mapper.ppucycle();
            }
        }
        if (nes.ppu.clock < 256 && nes.ppu.onscreen && nes.ppu.enabled) {
            nesppu_run_renderpixel();
        }
    }
}
