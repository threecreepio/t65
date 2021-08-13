#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "nes.h"
#include "apu.h"

const	unsigned char	LengthCounts[32] = {
	0x0A,0xFE,
	0x14,0x02,
	0x28,0x04,
	0x50,0x06,
	0xA0,0x08,
	0x3C,0x0A,
	0x0E,0x0C,
	0x1A,0x0E,

	0x0C,0x10,
	0x18,0x12,
	0x30,0x14,
	0x60,0x16,
	0xC0,0x18,
	0x48,0x1A,
	0x10,0x1C,
	0x20,0x1E
};

const	signed char	SquareDuty[4][8] = {
	{-4,-4,-4,-4,-4,-4,-4,+4},
	{-4,-4,-4,-4,-4,-4,+4,+4},
	{-4,-4,-4,-4,+4,+4,+4,+4},
	{+4,+4,+4,+4,+4,+4,-4,-4},
};
const	signed char	TriangleDuty[32] = {
	+7,+6,+5,+4,+3,+2,+1,+0,
	-1,-2,-3,-4,-5,-6,-7,-8,
	-8,-7,-6,-5,-4,-3,-2,-1,
	+0,+1,+2,+3,+4,+5,+6,+7,
};

struct nesapu_region REGIONS[2] = {
    {
        1789773, // (236.25f / 11.00f) / 12.00f
        { 7456,14912,22370,29828,37280 },
        { 0x1AC,0x17C,0x154,0x140,0x11E,0x0FE,0x0E2,0x0D6,0x0BE,0x0A0,0x08E,0x080,0x06A,0x054,0x048,0x036, }
    },
    {
        1662607, // (26.6017125f) / 16.00f
        { 8312,16626,24938,33252,41560 },
        { 0x18E,0x162,0x13C,0x12A,0x114,0x0EC,0x0D2,0x0C6, 0x0B0,0x094,0x084,0x076,0x062,0x04E,0x042,0x032, }
    },
};

#define FREQUENCY 44100
#define REGION (REGIONS[nes.cpu.region])

enum NESirq {
    IRQ_NONE  = 0b00000000,
    IRQ_FRAME = 0b00000001,
    IRQ_DPCM  = 0b00000010
};

void nesapu_initialize(void) {
    nesc_memset(&nes.apu, 0, sizeof(struct nesapu));
}

void nesapu_poweron(void) {
    nesapu_reset();
    nes.apu.cycles = 1;
    nes.apu.clock = 0;
    nes.cpu.irq &= ~(IRQ_FRAME | IRQ_DPCM);
}

void nesapu_reset(void) {
    // todo, not everything should be cleared on reset..
    nesc_memset(&nes.apu, 0, sizeof(struct nesapu));
	nes.apu.square0.cycles = 1;
	nes.apu.square0.envctr = 1;
	nes.apu.square0.bendctr = 1;
	nes.apu.square1.cycles = 1;
	nes.apu.square1.envctr = 1;
	nes.apu.square1.bendctr = 1;
	nes.apu.triangle.cycles = 1;
    nes.apu.noise.cycles = 1;
    nes.apu.noise.curd = 1;
    nes.apu.noise.envctr = 1;
    nes.apu.dpcm.cycles = 1;
    nes.apu.dpcm.bufempty = 1;
    nes.apu.dpcm.outbits = 8;
}

void nesapu_dpcmfetch(void) {
	nes.apu.dpcm.buffer = nescpu_readmem(nes.apu.dpcm.curAddr);
	nes.apu.dpcm.bufempty = 0;
	nes.apu.dpcm.fetching = 0;
	if (++nes.apu.dpcm.curAddr == 0x10000)
		nes.apu.dpcm.curAddr = 0x8000;
	if (!nes.apu.dpcm.lengthcontrol)
	{
		if (nes.apu.dpcm.wavehold)
		{
			nes.apu.dpcm.curAddr = 0xC000 | (nes.apu.dpcm.addr << 6);
			nes.apu.dpcm.lengthcontrol = (nes.apu.dpcm.len << 4) + 1;
		}
		else if (nes.apu.dpcm.doirq) {
            nes.cpu.wantirq |= IRQ_DPCM;
        }
	}
}

uint8_t nesapu_readmem(uint16_t addr) {
    uint8_t read;
    switch (addr & 0xFFF) {
        case 0x15:
            read =
                ((      nes.apu.square0.channel.lengthcontrol) ? 0x01 : 0) |
                ((      nes.apu.square1.channel.lengthcontrol) ? 0x02 : 0) |
                ((     nes.apu.triangle.channel.lengthcontrol) ? 0x04 : 0) |
                ((        nes.apu.noise.channel.lengthcontrol) ? 0x08 : 0) |
                ((         nes.apu.dpcm.lengthcontrol) ? 0x10 : 0) |
                ((nes.cpu.wantirq & IRQ_FRAME) ? 0x40 : 0) |
                ((nes.cpu.wantirq &  IRQ_DPCM) ? 0x80 : 0);
                nes.cpu.wantirq &= ~IRQ_FRAME;	// DPCM flag doesn't get reset
        case 0x016:
            read = nes.apu.lastread & 0xC0;
            read |= nes.input[0].read(&nes.input[0]);
            break;
        case 0x017:
            read = nes.apu.lastread & 0xC0;
            read |= nes.input[1].read(&nes.input[1]);
            break;
        default:
            read = nes.apu.lastread;
    }
    nes.apu.lastread = read;
    return read;
}

void nesapu_noisequarter() {
	if (nes.apu.noise.envclock)
	{
		nes.apu.noise.envclock = 0;
		nes.apu.noise.envelope = 0xF;
		nes.apu.noise.envctr = nes.apu.noise.volume;
	}
	else if (!nes.apu.noise.envctr--)
	{
		nes.apu.noise.envctr = nes.apu.noise.volume;
		if (nes.apu.noise.envelope)
			nes.apu.noise.envelope--;
		else	nes.apu.noise.envelope = nes.apu.noise.channel.wavehold ? 0xF : 0x0;
	}
	nes.apu.noise.vol = nes.apu.noise.envelope2 ? nes.apu.noise.volume : nes.apu.noise.envelope;
	if (nes.apu.noise.channel.lengthcontrol) {
		nes.apu.noise.pos = ((nes.apu.noise.curd & 0x4000) ? -2 : 2) * nes.apu.noise.vol;
    }
}

void nesapu_noisehalf(void) {
	if (nes.apu.noise.channel.lengthcontrol && !nes.apu.noise.channel.wavehold)
		nes.apu.noise.channel.lengthcontrol--;
}

void nesapu_square0quarter(void) {
	if (nes.apu.square0.envclock)
	{
		nes.apu.square0.envclock = 0;
		nes.apu.square0.envelope = 0xF;
		nes.apu.square0.envctr = nes.apu.square0.volume;
	}
	else if (!nes.apu.square0.envctr--)
	{
		nes.apu.square0.envctr = nes.apu.square0.volume;
		if (nes.apu.square0.envelope) {
			nes.apu.square0.envelope--;
        } else {
            nes.apu.square0.envelope = nes.apu.square0.channel.wavehold ? 0xF : 0x0;
        }
	}
	nes.apu.square0.vol = nes.apu.square0.envelope2 ? nes.apu.square0.volume : nes.apu.square0.envelope;
	nes.apu.square0.validfreq = (nes.apu.square0.freq >= 0x8) && ((nes.apu.square0.swpdir) || !((nes.apu.square0.freq + (nes.apu.square0.freq >> nes.apu.square0.swpstep)) & 0x800));
	nes.apu.square0.active = nes.apu.square0.channel.lengthcontrol && nes.apu.square0.validfreq;
	nes.apu.square0.pos = nes.apu.square0.active ? (SquareDuty[nes.apu.square0.duty][nes.apu.square0.curd] * nes.apu.square0.vol) : 0;
}

void nesapu_square0half(void) {
	if (!nes.apu.square0.bendctr--) {
		nes.apu.square0.bendctr = nes.apu.square0.swpspeed;
		if (nes.apu.square0.swpenab && nes.apu.square0.swpstep && nes.apu.square0.validfreq) {
			int sweep = nes.apu.square0.freq >> nes.apu.square0.swpstep;
			nes.apu.square0.freq += nes.apu.square0.swpdir ? ~sweep : sweep;
		}
	}
	if (nes.apu.square0.SwpClk) {
		nes.apu.square0.SwpClk = 0;
		nes.apu.square0.bendctr = nes.apu.square0.swpspeed;
	}
	if (nes.apu.square0.channel.lengthcontrol && !nes.apu.square0.channel.wavehold) {
		nes.apu.square0.channel.lengthcontrol--;
    }
	nes.apu.square0.validfreq = (nes.apu.square0.freq >= 0x8) && ((nes.apu.square0.swpdir) || !((nes.apu.square0.freq + (nes.apu.square0.freq >> nes.apu.square0.swpstep)) & 0x800));
	nes.apu.square0.active = nes.apu.square0.channel.lengthcontrol && nes.apu.square0.validfreq;
	nes.apu.square0.pos = nes.apu.square0.active ? (SquareDuty[nes.apu.square0.duty][nes.apu.square0.curd] * nes.apu.square0.vol) : 0;
}

void nesapu_writesquare0(uint16_t addr, uint8_t value) {
    switch (addr) {
        case 0:
            nes.apu.square0.volume = value & 0xF;
            nes.apu.square0.envelope2 = value & 0x10;
            nes.apu.square0.channel.race_wavehold = value & 0x20;
            nes.apu.square0.duty = (value >> 6) & 0x3;
            nes.apu.square0.vol = nes.apu.square0.envelope2
                ? nes.apu.square0.volume
                : nes.apu.square0.envelope;
            break;
        case 1:
            nes.apu.square0.swpstep = value & 0x07;
            nes.apu.square0.swpdir = value & 0x08;
            nes.apu.square0.swpspeed = (value >> 4) & 0x7;
            nes.apu.square0.swpenab = value & 0x80;
            nes.apu.square0.SwpClk = 1;
            break;
        case 2:
            nes.apu.square0.freq &= 0x700;
		    nes.apu.square0.freq |= value;
            break;
        case 3:
            nes.apu.square0.freq &= 0xFF;
            nes.apu.square0.freq |= (value & 0x7) << 8;
            nes.apu.square0.channel.race_lengthcontrol1 = LengthCounts[(value >> 3) & 0x1F];
            nes.apu.square0.channel.race_lengthcontrol2 = nes.apu.square0.channel.lengthcontrol;
            nes.apu.square0.curd = 0;
            nes.apu.square0.envclock = 1;
            break;
    }
	nes.apu.square0.validfreq = (nes.apu.square0.freq >= 0x8) && ((nes.apu.square0.swpdir) || !((nes.apu.square0.freq + (nes.apu.square0.freq >> nes.apu.square0.swpstep)) & 0x800));
	nes.apu.square0.active = nes.apu.square0.channel.lengthcontrol && nes.apu.square0.validfreq;
	nes.apu.square0.pos = nes.apu.square0.active ? (SquareDuty[nes.apu.square0.duty][nes.apu.square0.curd] * nes.apu.square0.vol) : 0;
}

void nesapu_square1quarter(void) {
	if (nes.apu.square1.envclock)
	{
		nes.apu.square1.envclock = 0;
		nes.apu.square1.envelope = 0xF;
		nes.apu.square1.envctr = nes.apu.square1.volume;
	}
	else if (!nes.apu.square1.envctr--)
	{
		nes.apu.square1.envctr = nes.apu.square1.volume;
		if (nes.apu.square1.envelope) {
			nes.apu.square1.envelope--;
        } else {
            nes.apu.square1.envelope = nes.apu.square1.channel.wavehold ? 0xF : 0x0;
        }
	}
	nes.apu.square1.vol = nes.apu.square1.envelope2 ? nes.apu.square1.volume : nes.apu.square1.envelope;
	nes.apu.square1.validfreq = (nes.apu.square1.freq >= 0x8) && ((nes.apu.square1.swpdir) || !((nes.apu.square1.freq + (nes.apu.square1.freq >> nes.apu.square1.swpstep)) & 0x800));
	nes.apu.square1.active = nes.apu.square1.channel.lengthcontrol && nes.apu.square1.validfreq;
	nes.apu.square1.pos = nes.apu.square1.active ? (SquareDuty[nes.apu.square1.duty][nes.apu.square1.curd] * nes.apu.square1.vol) : 0;
}

void nesapu_square1half(void) {
	if (!nes.apu.square1.bendctr--) {
		nes.apu.square1.bendctr = nes.apu.square1.swpspeed;
		if (nes.apu.square1.swpenab && nes.apu.square1.swpstep && nes.apu.square1.validfreq) {
			int sweep = nes.apu.square1.freq >> nes.apu.square1.swpstep;
			nes.apu.square1.freq += nes.apu.square1.swpdir ? ~sweep : sweep;
		}
	}
	if (nes.apu.square1.SwpClk) {
		nes.apu.square1.SwpClk = 0;
		nes.apu.square1.bendctr = nes.apu.square1.swpspeed;
	}
	if (nes.apu.square1.channel.lengthcontrol && !nes.apu.square1.channel.wavehold) {
		nes.apu.square1.channel.lengthcontrol--;
    }
	nes.apu.square1.validfreq = (nes.apu.square1.freq >= 0x8) && ((nes.apu.square1.swpdir) || !((nes.apu.square1.freq + (nes.apu.square1.freq >> nes.apu.square1.swpstep)) & 0x800));
	nes.apu.square1.active = nes.apu.square1.channel.lengthcontrol && nes.apu.square1.validfreq;
	nes.apu.square1.pos = nes.apu.square1.active ? (SquareDuty[nes.apu.square1.duty][nes.apu.square1.curd] * nes.apu.square1.vol) : 0;
}

void nesapu_writesquare1(uint16_t addr, uint8_t value) {
    switch (addr) {
        case 0:
            nes.apu.square1.volume = value & 0xF;
            nes.apu.square1.envelope2 = value & 0x10;
            nes.apu.square1.channel.race_wavehold = value & 0x20;
            nes.apu.square1.duty = (value >> 6) & 0x3;
            nes.apu.square1.vol = nes.apu.square1.envelope2
                ? nes.apu.square1.volume
                : nes.apu.square1.envelope;
            break;
        case 1:
            nes.apu.square1.swpstep = value & 0x07;
            nes.apu.square1.swpdir = value & 0x08;
            nes.apu.square1.swpspeed = (value >> 4) & 0x7;
            nes.apu.square1.swpenab = value & 0x80;
            nes.apu.square1.SwpClk = 1;
            break;
        case 2:
            nes.apu.square1.freq &= 0x700;
		    nes.apu.square1.freq |= value;
            break;
        case 3:
            nes.apu.square1.freq &= 0xFF;
            nes.apu.square1.freq |= (value & 0x7) << 8;
            nes.apu.square1.channel.race_lengthcontrol1 = LengthCounts[(value >> 3) & 0x1F];
            nes.apu.square1.channel.race_lengthcontrol2 = nes.apu.square1.channel.lengthcontrol;
            nes.apu.square1.curd = 0;
            nes.apu.square1.envclock = 1;
            break;
    }
	nes.apu.square1.validfreq = (nes.apu.square1.freq >= 0x8) && ((nes.apu.square1.swpdir) || !((nes.apu.square1.freq + (nes.apu.square1.freq >> nes.apu.square1.swpstep)) & 0x800));
	nes.apu.square1.active = nes.apu.square1.channel.lengthcontrol && nes.apu.square1.validfreq;
	nes.apu.square1.pos = nes.apu.square1.active ? (SquareDuty[nes.apu.square1.duty][nes.apu.square1.curd] * nes.apu.square1.vol) : 0;
}

void nesapu_trianglequarter(void) {
	if (nes.apu.triangle.lineclock)
        nes.apu.triangle.linecontrol = nes.apu.triangle.linear;
	else if (nes.apu.triangle.linecontrol)
		nes.apu.triangle.linecontrol--;
	if (!nes.apu.triangle.channel.wavehold)
		nes.apu.triangle.lineclock = 0;
	nes.apu.triangle.active = nes.apu.triangle.channel.lengthcontrol && nes.apu.triangle.linecontrol;
	if (nes.apu.triangle.freq < 4) {
		nes.apu.triangle.pos = 0;	// beyond hearing range
    } else {
        nes.apu.triangle.pos = TriangleDuty[nes.apu.triangle.curd] * 8;
    }
}

void nesapu_trianglehalf(void) {
	if (nes.apu.triangle.channel.lengthcontrol && !nes.apu.triangle.channel.wavehold)
		nes.apu.triangle.channel.lengthcontrol--;
	nes.apu.triangle.active = nes.apu.triangle.channel.lengthcontrol && nes.apu.triangle.linecontrol;
	if (nes.apu.triangle.freq < 4) {
		nes.apu.triangle.pos = 0;	// beyond hearing range
    } else {
        nes.apu.triangle.pos = TriangleDuty[nes.apu.triangle.curd] * 8;
    }
}

void nesapu_writetriangle(uint16_t addr, uint8_t value) {
    switch (addr) {
        case 0:
            nes.apu.triangle.linear = value & 0x7F;
            nes.apu.triangle.channel.race_wavehold = (value >> 7) & 0x1;
            break;
        case 2:
            nes.apu.triangle.freq &= 0x700;
            nes.apu.triangle.freq |= value;
            break;
        case 3:
            nes.apu.triangle.freq &= 0xFF;
            nes.apu.triangle.freq |= (value & 0x7) << 8;
            nes.apu.triangle.channel.race_lengthcontrol1 = LengthCounts[(value >> 3) & 0x1F];
            nes.apu.triangle.channel.race_lengthcontrol2 = nes.apu.triangle.channel.lengthcontrol;
            nes.apu.triangle.lineclock = 1;
            break;
    }
	nes.apu.triangle.active = nes.apu.triangle.channel.lengthcontrol && nes.apu.triangle.linecontrol;
	if (nes.apu.triangle.freq < 4) {
		nes.apu.triangle.pos = 0;	// beyond hearing range
    } else {
        nes.apu.triangle.pos = TriangleDuty[nes.apu.triangle.curd] * 8;
    }
}


void nesapu_writemem(uint16_t fulladdr, uint8_t value) {
	if (fulladdr < 0x018) {
		nes.apu.regs[fulladdr] = value;
    }

    uint16_t addr = fulladdr & 0x0FFF;
	switch (addr) {
        // square0
        case 0x000:
        case 0x001:
        case 0x002:
        case 0x003:
            nesapu_writesquare0(addr - 0x000, value);
            break;

        // square1
        case 0x004:
        case 0x005:
        case 0x006:
        case 0x007:
            nesapu_writesquare1(addr - 0x004, value);
            break;

        // triangle
        case 0x008:
        case 0x00A:
        case 0x00B:
            nesapu_writetriangle(addr - 0x008, value);
            break;

        // noise
        case 0x00C:
            nes.apu.noise.volume = value & 0x0F;
            nes.apu.noise.envelope2 = value & 0x10;
            nes.apu.noise.channel.race_wavehold = value & 0x20;
            nes.apu.noise.vol = nes.apu.noise.envelope2
                ? nes.apu.noise.volume
                : nes.apu.noise.envelope;
            if (nes.apu.noise.channel.lengthcontrol)
                nes.apu.noise.pos = ((nes.apu.noise.curd & 0x4000) ? -2 : 2) * nes.apu.noise.vol;
		    break;
        case 0x00E:
            nes.apu.noise.freq = value & 0xF;
		    nes.apu.noise.datatype = value & 0x80;
            break;
        case 0x00F:
        	nes.apu.noise.channel.race_lengthcontrol1 = LengthCounts[(value >> 3) & 0x1F];
			nes.apu.noise.channel.race_lengthcontrol2 = nes.apu.noise.channel.lengthcontrol;
            break;

        // dpcm
        case 0x010:
            nes.apu.dpcm.freq = value & 0xF;
            nes.apu.dpcm.wavehold = (value >> 6) & 0x1;
            nes.apu.dpcm.doirq = value >> 7;
            if (!nes.apu.dpcm.doirq)
                nes.cpu.wantirq &= ~IRQ_DPCM;
            break;
        case 0x011:
            nes.apu.dpcm.pcmdata = value & 0x7F;
		    nes.apu.dpcm.pos = (nes.apu.dpcm.pcmdata - 0x40) * 3;
		    break;
        case 0x012:
            nes.apu.dpcm.addr = value;
            break;
        case 0x013:
            nes.apu.dpcm.len = value;
            break;

        //
        case 0x014:
            nes.cpu.dma |= DMA_SPR;
            nes.cpu.dmapage = value;
            break;

        case 0x015:
            nes.apu.square0.enabled = value > 0;
            nes.apu.square1.enabled = value > 0;
            nes.apu.triangle.enabled = value > 0;
            nes.apu.noise.enabled = value > 0;
            if (value == 0) {
                nes.apu.square0.channel.lengthcontrol = 0;
                nes.apu.square1.channel.lengthcontrol = 0;
                nes.apu.triangle.channel.lengthcontrol = 0;
                nes.apu.noise.channel.lengthcontrol = 0;
                nes.apu.dpcm.lengthcontrol = 0;
            } else if (!nes.apu.dpcm.lengthcontrol) {
                nes.apu.dpcm.curAddr = 0xC000 | (addr << 6);
                nes.apu.dpcm.lengthcontrol = (nes.apu.dpcm.len << 4) + 1;
            }
            nes.cpu.wantirq &= ~IRQ_DPCM;
            break;

        case 0x016:	
            nes.input[0].write(&nes.input[0], value);
            nes.input[1].write(&nes.input[1], value);
            break;

        case 0x017:
            nes.apu.frame.bits = value & 0xC0;
            nes.apu.frame.zero = 2 + (nes.apu.clock & 1);
            if (nes.apu.frame.bits & 0x40) {
                nes.cpu.wantirq &= ~IRQ_FRAME;
            }
            break;
	}

}

void nesapu_race(void) {
    struct nesapu_channel *channels[4] = {
        &nes.apu.square0.channel,
        &nes.apu.square1.channel,
        &nes.apu.triangle.channel,
        &nes.apu.noise.channel,
    };
    for (int i=0; i<4; ++i) {
        struct nesapu_channel *channel = channels[i];
        channel->wavehold = channel->race_wavehold;
        if (channel->race_lengthcontrol1) {
            if (channel->lengthcontrol == channel->race_lengthcontrol2)
                channel->lengthcontrol = channel->race_lengthcontrol1;
            channel->race_lengthcontrol1 = 0;
        }
    }
}

#define chan_shared(channel) do { \
    channel.wavehold = channel.race_wavehold; \
    if (channel.race_lengthcontrol1) { \
        if (channel.lengthcontrol == channel.race_lengthcontrol2) \
            channel.lengthcontrol = channel.race_lengthcontrol1; \
        channel.race_lengthcontrol1 = 0; \
    } \
} while(0);

void nesapu_runcycle(void) {
    nes.apu.cycles += 1;


    // frame
	if (nes.apu.frame.cycles == REGION.framecycles[0]) {
		nes.apu.frame.quarter = 2;
	} else if (nes.apu.frame.cycles == REGION.framecycles[1]) {
		nes.apu.frame.quarter = 2;
		nes.apu.frame.half = 2;
	} else if (nes.apu.frame.cycles == REGION.framecycles[2]) {
		nes.apu.frame.quarter = 2;
    } else if (nes.apu.frame.cycles == REGION.framecycles[3]) {
		if (!(nes.apu.frame.bits & 0x80))
		{
			nes.apu.frame.quarter = 2;
			nes.apu.frame.half = 2;
			nes.apu.frame.irq = 3;
			nes.apu.frame.cycles = -2;
		}
    } else if (nes.apu.frame.cycles == REGION.framecycles[4]) {
		nes.apu.frame.quarter = 2;
		nes.apu.frame.half = 2;
		nes.apu.frame.cycles = -2;
	}
    nes.apu.frame.cycles += 1;
    if (nes.apu.frame.quarter && !(--nes.apu.frame.quarter)) {
        nesapu_square0quarter();
        nesapu_square1quarter();
        nesapu_trianglequarter();
        nesapu_noisequarter();
    }
    if (nes.apu.frame.half && !(--nes.apu.frame.half)) {
        nesapu_square0half();
        nesapu_square1half();
        nesapu_trianglehalf();
        nesapu_noisehalf();
    }
    if (nes.apu.frame.irq) {
        if (!nes.apu.frame.bits) {
			nes.cpu.wantirq |= IRQ_FRAME;
        }
        nes.apu.frame.irq -= 1;
    }
    if (nes.apu.frame.zero && !(--nes.apu.frame.zero)) {
		if (nes.apu.frame.bits & 0x80)
		{
			nes.apu.frame.quarter = 2;
			nes.apu.frame.half = 2;
		}
		nes.apu.frame.cycles = 0;
    }

    chan_shared(nes.apu.square0.channel);
    chan_shared(nes.apu.square1.channel);
    chan_shared(nes.apu.triangle.channel);
    chan_shared(nes.apu.noise.channel);


    // square0
	if (!(--nes.apu.square0.cycles)) {
		nes.apu.square0.cycles = nes.apu.square0.freq << 1;
		nes.apu.square0.curd = (nes.apu.square0.curd - 1) & 0x7;
		if (nes.apu.square0.active) {
			nes.apu.square0.pos = SquareDuty[nes.apu.square0.duty][nes.apu.square0.curd] * nes.apu.square0.vol;
        }
	}

    // square1
	if (!(--nes.apu.square1.cycles)) {
		nes.apu.square1.cycles = nes.apu.square1.freq << 1;
		nes.apu.square1.curd = (nes.apu.square1.curd - 1) & 0x7;
		if (nes.apu.square1.active) {
			nes.apu.square1.pos = SquareDuty[nes.apu.square1.duty][nes.apu.square1.curd] * nes.apu.square1.vol;
        }
	}
    
    // triangle
	if (!(--nes.apu.triangle.cycles)) {
		nes.apu.triangle.cycles = nes.apu.triangle.freq;
		if (nes.apu.triangle.active) {
			nes.apu.triangle.curd++;
			nes.apu.triangle.curd &= 0x1F;
			if (nes.apu.triangle.freq < 4) {
				nes.apu.triangle.pos = 0;
            } else {
                nes.apu.triangle.pos = TriangleDuty[nes.apu.triangle.curd] * 8;
            }
		}
	}

    // noise
	if (!(--nes.apu.noise.cycles)) {
		nes.apu.noise.cycles = REGION.frequencies[nes.apu.noise.freq];
		if (nes.apu.noise.datatype) {
			nes.apu.noise.curd = (nes.apu.noise.curd << 1)
                | (((nes.apu.noise.curd >> 14) ^ (nes.apu.noise.curd >> 8)) & 0x1);
        }
		else {
            nes.apu.noise.curd = (nes.apu.noise.curd << 1)
                | (((nes.apu.noise.curd >> 14) ^ (nes.apu.noise.curd >> 13)) & 0x1);
        }
		if (nes.apu.noise.channel.lengthcontrol) {
			nes.apu.noise.pos = ((nes.apu.noise.curd & 0x4000) ? -2 : 2) * nes.apu.noise.vol;
        }
	}

    // dpcm
    if (!(--nes.apu.dpcm.cycles)) {
        nes.apu.dpcm.cycles = REGION.frequencies[nes.apu.dpcm.freq];
        if (!nes.apu.dpcm.silenced) {
            if (nes.apu.dpcm.shiftreg & 1) {
                if (nes.apu.dpcm.pcmdata <= 0x7D) {
                    nes.apu.dpcm.pcmdata += 2;
                } else if (nes.apu.dpcm.pcmdata >= 0x02) {
                    nes.apu.dpcm.pcmdata -= 2;
                }
            }
            nes.apu.dpcm.shiftreg >>= 1;
            nes.apu.dpcm.pos = (nes.apu.dpcm.pcmdata - 0x40) * 3;
        }
        if (!(--nes.apu.dpcm.outbits)) {
            nes.apu.dpcm.outbits = 8;
            if (!nes.apu.dpcm.bufempty) {
                nes.apu.dpcm.shiftreg = nes.apu.dpcm.buffer;
				nes.apu.dpcm.bufempty = 1;
				nes.apu.dpcm.silenced = 0;
            } else {
				nes.apu.dpcm.silenced = 1;
            }
        }
    }

    if (nes.apu.dpcm.bufempty && !nes.apu.dpcm.fetching && nes.apu.dpcm.lengthcontrol && (nes.apu.clock & 1)) {
        nes.apu.dpcm.fetching = 1;
        nes.cpu.dma |= DMA_PCM;
        nes.apu.dpcm.lengthcontrol -= 1;
    }

    // end
	nes.apu.clock ++;

	int newbufferposition = FREQUENCY * nes.apu.cycles / REGION.hz;

}
