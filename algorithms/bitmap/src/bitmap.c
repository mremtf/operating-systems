#include "../include/bitmap.h"

struct bitmap {
    uint8_t *data;
    size_t bit_count, byte_count;
};

void bitmap_set(bitmap_t *bitmap, size_t bit) { bitmap->data[bit >> 3] |= 0x01 << (bit & 0x07); }

void bitmap_reset(bitmap_t *bitmap, size_t bit) { bitmap->data[bit >> 3] &= ~(0x01 << (bit & 0x07)); }

bool bitmap_test(bitmap_t *bitmap, size_t bit) { return bitmap->data[bit >> 3] & (0x01 << (bit & 0x07)); }
bool bitmap_at(bitmap_t *bitmap, size_t bit) { return bitmap_test(bitmap, bit); }

void bitmap_flip(bitmap_t *bitmap, size_t bit) { bitmap->data[bit >> 3] ^= (0x01 << (bit & 0x07)); }

size_t bitmap_get_bits(bitmap_t *bitmap) { return bitmap->bit_count; }
size_t bitmap_get_bytes(bitmap_t *bitmap) { return bitmap->byte_count; }
uint8_t *bitmap_get_data(bitmap_t *bitmap) { return bitmap->data; }

void bitmap_format(bitmap_t *bitmap, uint8_t pattern) { memset(bitmap->data, pattern, bitmap->byte_count); }

bitmap_t *bitmap_initialize(size_t n_bits) {
    if (n_bits) { // must be non-zero
        bitmap_t *bitmap = (bitmap_t *) malloc(sizeof(bitmap_t));
        if (bitmap) {
            bitmap->bit_count = n_bits;
            bitmap->byte_count = n_bits >> 3;
            bitmap->byte_count += (n_bits & 0x07 ? 1 : 0);
            bitmap->data = (uint8_t *)calloc(bitmap->byte_count, 1);

            if (bitmap->data) {
                return bitmap;
            }
            free(bitmap);
        }
    }
    return NULL;
}

bitmap_t *bitmap_import(size_t n_bits, uint8_t *bitmap_data) {
    bitmap_t *bitmap = bitmap_initialize(n_bits);
    if (bitmap && bitmap_data) {
        memcpy(bitmap->data, bitmap_data, bitmap->byte_count);
        return bitmap;
    }
    free(bitmap);
    return NULL;
}

void bitmap_destroy(bitmap_t *bitmap) {
    if (bitmap) {
        free(bitmap->data);
        free(bitmap);
    }
}
