#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "nes.h"
#include "mappers.c"

int nes_loadrom(const char *data, uint32_t size) {
    const char *bin = data;
    char header[0x10];
    memcpy(header, bin, 0x10);
    bin += 0x10;
    if (strncmp(header, "NES\x1A", 4)) {
        // assume NROM
        memset(header, 0, 0x10);
        bin -= 0x10;
        header[0x4] = 2;
        header[0x5] = 0;
        //fprintf(stderr, "Invalid INES Header\n");
        //return 1;
    }

    nes.rominfo.type = 0;
    nes.rominfo.ines_version = 1;
    nes.rominfo.ines_prgsize = header[0x4];
    nes.rominfo.ines_chrsize = header[0x5];
    nes.rominfo.ines_mapper = ((header[0x6] & 0xF0) >> 4) | (header[0x7] & 0xF0);
    nes.rominfo.ines_flags = (header[0x6] & 0x0F) | ((header[0x7] & 0x0F) << 4);

    // iNES v2.0
    if ((header[0x07] & 0x0C) == 0x08) {
        nes.rominfo.ines_version = 2;
        nes.rominfo.ines_mapper |= (header[0x08] & 0x0F) << 8;
        nes.rominfo.ines2_submapper = (header[0x08] & 0xF0) >> 4;
        nes.rominfo.ines_prgsize |= (header[0x9] & 0x0F) << 8;
        nes.rominfo.ines_chrsize |= (header[0x9] & 0xF0) << 4;
        nes.rominfo.ines2_prgram = header[0xA];
        nes.rominfo.ines2_chrram = header[0xB];
        nes.rominfo.ines2_tvmode = header[0xC];
        nes.rominfo.ines2_vsdata = header[0xD];
    }

    int prgsize = nes.rominfo.ines_prgsize * 0x4000;
    int chrsize = nes.rominfo.ines_chrsize * 0x2000;

    memcpy(&nes.prgrom, bin, prgsize);
    bin += prgsize;
    memcpy(&nes.chrrom, bin, chrsize);
    bin += chrsize;

    // missing vectors
    if (size < 0x8000) {
        nes.prgrom[0x7][0xFFA] = 0x00;
        nes.prgrom[0x7][0xFFB] = 0x80;
        nes.prgrom[0x7][0xFFC] = 0x00;
        nes.prgrom[0x7][0xFFD] = 0x80;
    }

    nes.mapper.poweron = 0;
    nes.mapper.cpucycle = 0;
    nes.mapper.ppucycle = 0;
    nes.mapper.reset = 0;
    switch (nes.rominfo.ines_mapper) {
        case 0:
        case 2:
            mapper000_initialize();
            return 0;
        default:
            printf("unsupported mapper %d\n", nes.rominfo.ines_mapper);
            exit(2);
            return 2;
    }
}