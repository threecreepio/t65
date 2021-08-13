#pragma once
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include "apu.h"
struct nes;

#define CPUSTEP_OK 0
#define CPUSTEP_NMI 0x80
#define CPUSTEP_BRK 2
#define CPUSTEP_CLD 4
#define CPUSTEP_INVALID 3

uint16_t NESCPU_OPS[0x100];

enum NESAddressingModes {
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

enum NESInstructions {
    IN_ADC,IN_AND,IN_ASL,IN_ASLA,IN_BCC,IN_BCS,IN_BEQ,IN_BIT,IN_BMI,IN_BNE,
    IN_BPL,IN_BRK,IN_BVC,IN_BVS,IN_CLC,IN_CLD,IN_CLI,IN_CLV,IN_CMP,IN_CPX,
    IN_CPY,IN_DEC,IN_DEX,IN_DEY,IN_EOR,IN_INC,IN_INX,IN_INY,IN_JMP,IN_JMPI,
    IN_JSR,IN_LDA,IN_LDX,IN_LDY,IN_LSR,IN_LSRA,IN_NOP,IN_ORA,IN_PHA,IN_PHP,
    IN_PLA,IN_PLP,IN_ROL,IN_ROLA,IN_ROR,IN_RORA,IN_RTI,IN_RTS,IN_SBC,IN_SEC,
    IN_SED,IN_SEI,IN_STA,IN_STX,IN_STY,IN_TAX,IN_TAY,IN_TSX,IN_TXA,IN_TXS,
    IN_TYA,IX_AAC,IX_ARR,IX_ASR,IX_ATX,IX_AXS,IX_DCP,IX_HLT,IX_ISB,IX_LAX,
    IX_NOP,IX_NOP2,IX_RLA,IX_RRA,IX_SAX,IX_SBC,IX_SLO,IX_SRE,IX_UNK,
};

enum NESDma {
    DMA_PCM = 0b00000001,
    DMA_SPR = 0b00000010
};

enum NESRegion {
    NESRegionNTSC = 0,
    NESRegionPAL = 1
};

struct nescpu_flagbits {
    bool c  : 1;
    bool z  : 1;
    bool i  : 1;
    bool d  : 1;
    bool b1 : 1;
    bool b2 : 1;
    bool v  : 1;
    bool n  : 1;
};

union nescpu_flags {
    struct nescpu_flagbits bits;
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

struct nesapu_region {
    uint32_t hz;
    uint16_t framecycles[5];
    uint16_t frequencies[16];
};

struct nesapu_frame {
	uint8_t bits;
	int32_t cycles;
	int32_t quarter, half, irq, zero;
};

struct nesapu_dpcm {
	uint8_t freq, wavehold, doirq, pcmdata, addr, len;
	uint32_t curAddr, sampleLen;
	uint8_t silenced, bufempty, fetching;
	uint8_t shiftreg, outbits, buffer;
	uint32_t lengthcontrol;	// short
	uint32_t cycles;	// short
	int32_t pos;
};

struct nesapu_channel {
	uint8_t wavehold, lengthcontrol, race_wavehold, race_lengthcontrol1, race_lengthcontrol2;
};

struct nesapu_noise {
	struct nesapu_channel channel;
	uint32_t freq;	// short
	uint32_t curd;	// short
	uint8_t volume, envelope2, datatype;
	uint8_t vol;
	uint8_t envctr, envelope;
	uint8_t enabled;
	uint8_t envclock;
	uint32_t cycles;	// short
	int32_t pos;
};

struct nesapu_square {
	struct nesapu_channel channel;
	uint8_t volume, envelope2, duty, swpspeed, swpdir, swpstep, swpenab;
	uint32_t freq;	// short
	uint8_t vol;
	uint8_t curd;
	uint8_t envctr, envelope, bendctr;
	uint8_t enabled, validfreq, active;
	uint8_t envclock, SwpClk;
	uint32_t cycles;	// short
	int32_t pos;
};

struct nesapu_triangle {
	struct nesapu_channel channel;
	uint8_t linear;
	uint32_t freq;
	uint8_t curd;
	uint8_t linecontrol;
	uint8_t enabled, active;
	uint8_t lineclock;
	uint32_t cycles;
	int32_t pos;
};

struct nesapu {
    uint32_t cycles;
    uint32_t bufpos;
    short *buffer;
    uint32_t buflen;
    uint8_t regs[0x18];
    uint32_t mhz;
    uint32_t clock;
    uint8_t lastread;

    struct nesapu_frame frame;
    struct nesapu_noise noise;
    struct nesapu_dpcm dpcm;
    struct nesapu_triangle triangle;
    struct nesapu_square square0;
    struct nesapu_square square1;
};

struct nescpu {
    uint8_t *bankdata[0x10];
    uint8_t bankflags[0x10];
    uint8_t ram[0x800];
    uint8_t ra, rx, ry, rsp;
    uint8_t irq, nmi;
    union nescpu_flags ps;
    union splitreg pc;
    uint32_t cycles;
    uint32_t dbg_nmicounter;
    enum NESRegion region;

    bool lastnmi, lastirq;
    bool wantnmi, wantirq;
    uint8_t dma;
    uint8_t dmapage;
    uint8_t lastread;
};

struct nesppu {
    uint8_t *bankdata[0x10];
    uint8_t bankflags[0x10];
    int framesrendered;

    bool wantsblit;
    uint8_t readlatch;
    bool hvtog;
    uint32_t ops;

    uint8_t screen[256 * 240];
    int screenat;

    uint8_t renderdata[4];
    uint8_t palette[0x20];
    uint8_t vram[0x2][0x400];
    uint8_t tiledata[272];
    uint8_t ioval;
    uint8_t buf2007;
    uint8_t intx;
    int32_t grayscale, coloremphasis;
    uint32_t renderaddr, patternaddr;
    uint32_t vramaddr, intreg;
    int32_t scanline, scanlineend;
    int32_t clock;
    uint8_t iomode, reg2000, reg2001, reg2002;

    uint8_t sprite[0x121];
    uint8_t *sprbuff;
    uint8_t sprdata[8][10];
    int spraddr;
    uint8_t sprtmp;
    uint16_t sprptr;
    int spraddrh;
    uint8_t sprcount;
    uint8_t sprstate;
    uint8_t sprcount_tmp;
    bool sprzero;


    bool spr0line;
    bool scanlineshort;
    bool onscreen;
    bool rendering;
    bool enabled;
};

struct nesinput {
    uint8_t (*read)(struct nesinput *self);
    void (*write)(struct nesinput *self, uint8_t value);
};

struct nesmapper {
    void (*writemem)(uint16_t addr, uint8_t value);
    void (*poweron)(void);
    void (*reset)(void);
    void (*cpucycle)(void);
    void (*ppucycle)(void);
};

struct nesrominfo {
    int type;
    int ines_version;
    uint8_t ines_prgsize;
    uint8_t ines_chrsize;
    uint8_t ines_mapper;
    uint8_t ines_flags;

    uint8_t ines2_submapper;
    uint8_t ines2_prgram;
    uint8_t ines2_chrram;
    uint8_t ines2_tvmode;
    uint8_t ines2_vsdata;
};


struct nes {
    struct nescpu cpu;
    struct nesrominfo rominfo;
    struct nesmapper mapper;
    struct nesppu ppu;
    struct nesapu apu;
    struct nesinput input[2];

    uint8_t breaking;
    uint8_t prgrom[0x800][0x1000];
    uint8_t prgram[0x100][0x1000];
    uint8_t chrrom[0x1000][0x400];
    uint8_t chrram[0x100][0x400];
};

int nes_loadrom(const char *data, uint32_t size);
void nes_initialize(void);
void nes_reset(bool hard);
void nes_step(void);

void nescpu_initialize(void);
void nescpu_poweron(void);
void nescpu_reset(void);
uint8_t nescpu_readmem(uint16_t addr);
void nescpu_writemem(uint16_t addr, uint8_t value);
void nescpu_printstate(void);
int nescpu_step(void);

void nesppu_initialize(void);
void nesppu_poweron(void);
void nesppu_reset(void);
void nesppu_runcycle(void);
uint8_t nesppu_readmem(uint16_t addr);
void nesppu_writemem(uint16_t addr, uint8_t value);

void nesapu_initialize(void);
void nesapu_poweron(void);
void nesapu_reset(void);
void nesapu_runcycle(void);
uint8_t nesapu_readmem(uint16_t addr);
void nesapu_writemem(uint16_t addr, uint8_t value);
void nesapu_dpcmfetch(void);

int nes_frameadvance(void);

void nesc_memset(void *data, char value, uint32_t count);
void nesc_memcpy(void *dest, const void *src, uint32_t count);

extern struct nes nes;
