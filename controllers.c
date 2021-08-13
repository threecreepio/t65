#include <stdint.h>

uint8_t nesinput_read(struct nes *nes, uint8_t number);
uint8_t nesinput_write(struct nes *nes, uint8_t number, uint8_t value);
