#include <stdint.h>

void nesc_memset(void *data, char value, uint32_t count) {
    for (uint32_t i=0; i<count; ++i) {
        ((char *)data)[i] = value;
    }
}

void nesc_memcpy(void *dest, const void *src, uint32_t count) {
    for (uint32_t i=0; i<count; ++i) {
        ((char*)dest)[i] = ((char*)src)[i];
    }
}
