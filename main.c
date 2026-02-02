#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "sys.h"
#include "cpu.c"
#include "cpu-debug.c"
#include "asm6.c"

struct sys sys = {0};

int trace = 1;
int romloaded = 0;

int mode = 0;
void handle_sigint(int s) {
    if (mode == 27) {
        syscpu_printmem();
        exit(0);
    }
    mode = 27;
}

void t65_writemem(uint16_t address, uint8_t value) {
    if ((address & 0xF000) != 0x5000) return;
    uint8_t reg = address & 0x00FF;
    // 5000 is char output
    if (reg == 0x00) {
        printf("%c", value);
        fflush(stdout);
    }
    // 5001 toggles tracing and clears cycle counters
    else if (reg == 0x01) {
        sys.cpu.cycles = 0;
        sys.cpu.cycles2 = 0;
        trace = value ? 1 : 0;
    }
    // 5002 triggers a break
    else if (reg == 0x02) {
        if (!trace) {
            sys.cpu.cycles = 0;
            sys.cpu.cycles2 = 0;
        }
        trace = 1;
        mode = 27;
    }
    // 5003 exits the program
    else if (reg == 0x03) {
        if (trace) {
            syscpu_printmem();
        }
        exit((int) value);
    }
}

void t65_showhelp(void) {
    fprintf(stderr, "");
    fprintf(stderr, "Usage:  t65 [-options] infile\n");
    fprintf(stderr, "    -q,--quiet  quiet mode, disables cpu tracing");
    fprintf(stderr, "");
}

int main(int argc, char **argv) {
    if (argc < 2) {
        t65_showhelp();
        exit(1);
    }

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

        if (romloaded) {
            fprintf(stderr, "Multiple input files specified\n");
            exit(1);
        }
        
        // attempt to assemble the input file
        int r = asm6_main(4, (char*[]){ argv[0], "-q", argv[i], ".t65.rom" });
        FILE *f = fopen(".t65.rom", "rb");
        if (!f || r != 0) {
            fprintf(stderr, "\n");
            exit(2);
        }
        // read assembled data
        int len = fread(&sys.rom[0], 1, sizeof(sys.rom), f);
        if (len <= 0) {
            fprintf(stderr, "Error reading assembled ROM\n");
            exit(3);
        }
        fclose(f);
        remove(".t65.rom");
        romloaded = 1;
    }
    
    if (romloaded == 0) {
        fprintf(stderr, "No input file specified\n");
        exit(1);
    }
    
    signal (SIGINT, handle_sigint);

    sys.writemem = t65_writemem;

    sys_reset(true);
    
    while (1) {
        if (trace) {
            syscpu_printstate();
        }
        int rv = syscpu_step();
        if (mode == 27) {
            // user requested break
            rv = CPUSTEP_CTRLC;
        }
        if (rv > 0 && !trace && (rv == CPUSTEP_CTRLC || rv == CPUSTEP_BRK)) {
            // if not tracing, we exit on any break condition
            break;
        }
        if (rv > 0) {
            mode = 27;
            if (rv == CPUSTEP_RTS) {
                break;
            }
            if (rv == CPUSTEP_BRK) {
                printf("BRK instruction. ");
            }
            printf("ENTER to step, R to run. Ctrl-C to exit.");

            // wait for user input
            char c[0x100];
            fgets(c, sizeof(c), stdin);
            if (c[0] == 'R' || c[0] == 'r') {
                // return to run mode
                mode = 0;
            }
        }
    }
    if (trace) {
        syscpu_printmem();
        printf("\n");
    }
    return sys.cpu.ra;
}
