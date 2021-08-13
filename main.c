#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <time.h>
#include "nes.h"
#include "ines.c"
#include "cpu.c"
#include "cpu-debug.c"
#include "apu.c"
#include "nes.c"
#include "ppu.c"
#include "utils.c"
#include "controllers.c"
#include <signal.h> 

struct nes nes = {0};

long timediff(clock_t t1, clock_t t2) {
    long elapsed;
    elapsed = ((double)t2 - t1) / CLOCKS_PER_SEC * 1000;
    return elapsed;
}

char palmap[64][3] = {
        0x80, 0x80, 0x80,
        0x00, 0x3D, 0xA6,
        0x00, 0x12, 0xB0,
        0x44, 0x00, 0x96,
        0xA1, 0x00, 0x5E,
        0xC7, 0x00, 0x28,
        0xBA, 0x06, 0x00,
        0x8C, 0x17, 0x00,
        0x5C, 0x2F, 0x00,
        0x10, 0x45, 0x00,
        0x05, 0x4A, 0x00,
        0x00, 0x47, 0x2E,
        0x00, 0x41, 0x66,
        0x00, 0x00, 0x00,
        0x05, 0x05, 0x05,
        0x05, 0x05, 0x05,
        0xC7, 0xC7, 0xC7,
        0x00, 0x77, 0xFF,
        0x21, 0x55, 0xFF,
        0x82, 0x37, 0xFA,
        0xEB, 0x2F, 0xB5,
        0xFF, 0x29, 0x50,
        0xFF, 0x22, 0x00,
        0xD6, 0x32, 0x00,
        0xC4, 0x62, 0x00,
        0x35, 0x80, 0x00,
        0x05, 0x8F, 0x00,
        0x00, 0x8A, 0x55,
        0x00, 0x99, 0xCC,
        0x21, 0x21, 0x21,
        0x09, 0x09, 0x09,
        0x09, 0x09, 0x09,
        0xFF, 0xFF, 0xFF,
        0x0F, 0xD7, 0xFF,
        0x69, 0xA2, 0xFF,
        0xD4, 0x80, 0xFF,
        0xFF, 0x45, 0xF3,
        0xFF, 0x61, 0x8B,
        0xFF, 0x88, 0x33,
        0xFF, 0x9C, 0x12,
        0xFA, 0xBC, 0x20,
        0x9F, 0xE3, 0x0E,
        0x2B, 0xF0, 0x35,
        0x0C, 0xF0, 0xA4,
        0x05, 0xFB, 0xFF,
        0x5E, 0x5E, 0x5E,
        0x0D, 0x0D, 0x0D,
        0x0D, 0x0D, 0x0D,
        0xFF, 0xFF, 0xFF,
        0xA6, 0xFC, 0xFF,
        0xB3, 0xEC, 0xFF,
        0xDA, 0xAB, 0xEB,
        0xFF, 0xA8, 0xF9,
        0xFF, 0xAB, 0xB3,
        0xFF, 0xD2, 0xB0,
        0xFF, 0xEF, 0xA6,
        0xFF, 0xF7, 0x9C,
        0xD7, 0xE8, 0x95,
        0xA6, 0xED, 0xAF,
        0xA2, 0xF2, 0xDA,
        0x99, 0xFF, 0xFC,
        0xDD, 0xDD, 0xDD,
        0x11, 0x11, 0x11,
        0x11, 0x11, 0x11
};


void writescreen() {
    int w = 256;
    int h = 240;
    uint8_t *src = nes.ppu.screen;
    FILE *screen = fopen("screen.ppm", "wb");
    fprintf(screen, "P6\n%d %d\n255\n", w, h);
    for (int i=0; i<(256 * 240); ++i) {
        fwrite(&palmap[src[i]], 1, 3, screen);
    }
    fclose(screen);
}


int strobe = 0;
int bits = 0;
int bitptr = 0;

uint8_t standardctl_read(struct nesinput *input) {
	unsigned char result;
	if (strobe) {
		bitptr= 0;
		result = (unsigned char)(bits & 1);
	} else {
		if (bitptr < 8) {
			result = (unsigned char)(bits >> bitptr++) & 1;
        } else {
            result = 1;
        }
	}
	return result;
}

void standardctl_write(struct nesinput *input, uint8_t value) {
	if (strobe || (value & 1)) {
        strobe = value & 1;
		bitptr = 0;
	}
}

uint8_t noop_read(struct nesinput *input) {
    return 0;
}

void noop_write(struct nesinput *input, uint8_t value) {
}

int mode = 0;
void handle_sigint(int s) {
    if (mode == 27) {
        nescpu_printmem();
        exit(0);
    }
    mode = 27;
}

int main(int argc, char **argv) {
    nes_initialize();
    FILE *f = fopen(argv[1], "r");
    if (!f) {
        fprintf(stderr, "Could not open ROM file\n");
        exit(2);
    }

    char data[0x81920];
    int len = fread(&data, 0x81920, 1, f);
    fclose(f);

    if (nes_loadrom(&data[0], len)) {
        fprintf(stderr, "Could not load ROM\n");
        exit(1);
    }
    nes_reset(true);
    nes.input[0].read = standardctl_read;
    nes.input[1].read = noop_read;
    nes.input[0].write = standardctl_write;
    nes.input[1].write = noop_write;

    signal (SIGINT, handle_sigint);
    mode = 0;

    while (1) {
        int f = nes.ppu.framesrendered;
        nescpu_printstate();
        int rv = nescpu_step() | nes.breaking;
        nes.breaking = 0;
        if (rv > 0) {
            printf("BREAK");
            if ((rv & CPUSTEP_NMI) == CPUSTEP_NMI) {
                printf(" NMI");
                writescreen();
            }
            if ((rv & 0x7F) == CPUSTEP_BRK) {
                printf(" BRK instruction");
            }
            if ((rv & 0x7F) == CPUSTEP_CLD) {
                printf(" CLD instruction");
            }
            if ((rv & 0x7F) == CPUSTEP_INVALID) {
                printf(" Undocumented instruction");
            }
            printf(". ENTER to run until next BRK, Ctrl-C to exit.\n");
            printf("\n");
        }
        if (mode == 27 || rv > 0) {
            mode = 27;
            mode = getc(stdin);
        }
    }
    nescpu_printmem();
    printf("\n");
    return 0;
}