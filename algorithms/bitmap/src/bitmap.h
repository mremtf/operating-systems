#ifndef BITMAP_H__
#define BITMAP_H__

typedef struct bitmap bitmap_t;

// Sets requested bit in bitmap
void bitmap_set(bitmap_t* bitmap, size_t bit);

// Clears requested bit in bitmap
void bitmap_unset(bitmap_t* bitmap, size_t bit);

// Returns bit in bitmap
bool bitmap_test(bitmap_t* bitmap, size_t bit);
bool bitmap_at(bitmap_t* bitmap, size_t bit);

// Flips bit in bitmap
void bitmap_flip(bitmap_t* bitmap, size_t bit);

// Resets bitmap to the desired pattern
void bitmap_reset(bitmap_t* bitmap, uint8_t pattern);

// Gets total number of bits in bitmap
size_t bitmap_get_bits(bitmap_t* bitmap);

// Gets total number of bytes in bitmap
size_t bitmap_get_bytes(bitmap_t* bitmap);

// Gets pointer for writing bitmap to file
uint8_t* bitmap_get_data(bitmap_t* bitmap);

// Creates a new bitmap with the provided data and size
bool bitmap_import(bitmap_t** bitmap, size_t n_bits, uint8_t* bitmap_data);

// Creates a bitmap to contain n bits in the desired pattern
bool bitmap_initialize(bitmap_t** bitmap, size_t n_bits, uint8_t pattern);

// Destructs and frees bitmap
void bitmap_destroy(bitmap_t* bitmap);


#endif