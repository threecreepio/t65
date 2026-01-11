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
#include "asm6.c"
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

char ines_header[0x10] = {
    0x4E, 0x45, 0x53, 0x1A, // "NES" and EOF
    0x01,                   // PRG ROM size in 16KB units
    0x00,                   // CHR ROM size in 8KB units
    0x00,                   // Flags 6
    0x00,                   // Flags 7
    0x00,                   // PRG RAM size in 8KB units
    0x00,                   // Flags 9
    0x00,                   // Flags 10
    0x00, 0x00, 0x00, 0x00  // Unused padding bytes
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

uint16_t debugprint_ptr = 0;

void t65_writemem(uint16_t address, uint8_t value) {
    // printf("handling writemem to %04X = %02X\n", address, value);
    if ((address & 0xF000) == 0x5000) {
        uint8_t reg = address & 0x000F;
        if (reg == 0x00) {
            printf("%c", value);
            fflush(stdout);
        }
    }
}

void t65_showhelp(void) {
    puts("");
    puts("Usage:  t65 [-options] infile\n");
    puts("    -q,--quiet  quiet mode, disables cpu tracing");
    puts("");
}


int main(int argc, char **argv) {
    nes_initialize();
    
    if (argc < 2) {
        t65_showhelp();
        exit(1);
    }
    
    char romdata[0x100000];
    int romloaded = 0;
    int trace = 1;
    
    for (int i=1; i<argc; ++i) {
        if (argv[i] == NULL) continue;
        if (argv[i][0] == '-') {
            // check for -q or --quiet
            if (strcmp(argv[i], "-q") == 0 || strcmp(argv[i], "--quiet") == 0) {
                trace = 0;
                continue;
            }
            fprintf(stderr, "Unknown option: %s\n", argv[i]);
            exit(1);
        }
        
        // check file extension
        const char *ext = strrchr(argv[i], '.');
        if (ext && (strcmp(ext, ".asm") == 0 || strcmp(ext, ".s") == 0)) {
            int r = asm6_main(4, (char*[]){ argv[0], "-q", argv[i], ".t65.rom" });
            FILE *f = fopen(".t65.rom", "rb");
            if (!f || r != 0) {
                fprintf(stderr, "\n");
                exit(2);
            }
            // copy ines header to romdata
            memcpy(&romdata[0], ines_header, 0x10);
            // read assembled rom data
            int len = fread(&romdata[0x10], 1, 0x100000, f);
            if (nes_loadrom(&romdata[0], len + 0x10)) {
                fprintf(stderr, "Could not load assembled ROM\n");
                exit(1);
            }
            romloaded = 1;
            fclose(f);
            remove(".t65.rom");
        } else {
            FILE *f = fopen(argv[i], "rb");
            if (!f) {
                fprintf(stderr, "Could not open ROM file: %s\n", argv[i]);
                exit(1);
            }
            int len = fread(&romdata[0], 1, 0x100000, f);
            if (nes_loadrom(&romdata[0], len)) {
                fprintf(stderr, "Could not load ROM file: %s\n", argv[i]);
                exit(1);
            }
            romloaded = 1;
            fclose(f);
        }
    }
    
    if (!romloaded) {
        fprintf(stderr, "No input file specified\n");
        exit(1);
    }
    
    nes_reset(true);
    nes.mapper.writemem = t65_writemem;
    nes.input[0].read = standardctl_read;
    nes.input[1].read = noop_read;
    nes.input[0].write = standardctl_write;
    nes.input[1].write = noop_write;
    
    signal (SIGINT, handle_sigint);
    mode = 0;
    
    while (1) {
        if (trace) nescpu_printstate();
        int rv = nescpu_step();
        if (rv > 0) {
            if ((rv & CPUSTEP_NMI) == CPUSTEP_NMI) {
                writescreen();
            }
            if ((rv & 0x7F) == CPUSTEP_RTS) {
                break;
            }
            if ((rv & 0x7F) == CPUSTEP_BRK) {
                printf("BRK instruction. ");
            }
            if ((rv & 0x7F) == CPUSTEP_NOP) {
                printf("NOP instruction. ");
            }
            printf("ENTER to continue, Ctrl-C to exit.");
        }
        if (mode == 27 || rv > 0) {
            mode = 27;
            mode = getc(stdin);
        }
    }
    if (trace) {
        nescpu_printmem();
        printf("\n");
    }
    return nes.cpu.ra;
}
