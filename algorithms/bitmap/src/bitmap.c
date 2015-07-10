#include <stdint.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdbool.h>


struct bitmap {
    uint8_t* data;
    size_t bit_count, byte_count;
}

void bitmap_set(bitmap_t* bitmap, size_t bit) { bitmap->data[bit >> 3] |= 0x01 << (bit & 0x07); }

void bitmap_unset(bitmap_t* bitmap, size_t bit) { bitmap->data[bit >> 3] &= ~(0x01 << (bit & 0x07)); }

bool bitmap_test(bitmap_t* bitmap, size_t bit) { return bitmap->data[bit >> 3] & (0x01 << (bit & 0x07)); }
bool bitmap_at(bitmap_t* bitmap, size_t bit) { return bitmap_test(bitmap, bit); }

void bitmap_flip(bitmap_t* bitmap, size_t bit) { bitmap->data[bit >> 3] ^= (0x01 << (bit & 0x07)); }

size_t bitmap_get_bits(bitmap_t* bitmap) { return bitmap->bit_count; }
size_t bitmap_get_bytes(bitmap_t* bitmap) { return bitmap->byte_count; }
uint8_t* bitmap_get_data(bitmap_t* bitmap) { return bitmap->data; }

void bitmap_reset(bitmap_t* bitmap, uint8_t pattern) { memset(bitmap->data, pattern, byte_count); }

bool bitmap_initialize(bitmap_t** bitmap, size_t n_bits, uint8_t pattern) {
    if (bitmap) {
        if (n_bits) { // must be non-zero
            *bitmap = (bitmap_t*)malloc(sizeof(bitmap_t));
            if (*bitmap) {
                (*bitmap)->bit_count = n_bits;
                (*bitmap)->byte_count = n_bits >> 3;
                (*bitmap)->byte_count += (n_bits & 0x07 ? 1 : 0);
                (*bitmap)->data = (uint8_t*)malloc(byte_count);

                if ((*bitmap)->data) {
                    memset((*bitmap)data, pattern, byte_count);
                    return true;
                }
                free((*bitmap)->data);
                free(*bitmap);
            }
        }
    }
    return false;
}

bool bitmap_import(bitmap_t** bitmap, size_t n_bits, uint8_t* bitmap_data) {
    if (bitmap_initialize(bitmap, n_bits, 0x00)) {
        memcpy((*bitmap)->data, bitmap_data, (*bitmap)->byte_count);
        return true;
    }
    return false;
}

void bitmap_destroy(bitmap_t* bitmap) {
    if (bitmap) {
        free(bitmap->data);
        free(bitmap);
    }
}
