#include "../include/bitmap.h"
#include "../src/bitmap.c"

#include <assert.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

/*
    // Sets requested bit in bitmap
    void bitmap_set(bitmap_t* bitmap, size_t bit);

    1. Set arbitrary bit in byte for all bits
    2. Set first bit
    3. Set last bit

    // Clears requested bit in bitmap
    void bitmap_reset(bitmap_t* bitmap, size_t bit);

    4. Clear arbitrary bit in byte for all bits
    5. Clear first bit
    6. Clear last bit

    // Returns bit in bitmap
    bool bitmap_test(bitmap_t* bitmap, size_t bit);
    bool bitmap_at(bitmap_t* bitmap, size_t bit);

    7. Get arbitrary bit in byte for all bits
    8. Get first bit
    9. Get last bit

    // Flips bit in bitmap
    void bitmap_flip(bitmap_t* bitmap, size_t bit);

    10. Get arbitrary bit in byte for all bits
    11. Get first bit
    12. Get last bit

    // Resets bitmap contents to the desired pattern
    void bitmap_format(bitmap_t* bitmap, uint8_t pattern);

    13. Set pattern and verify

    // Gets total number of bits in bitmap
    size_t bitmap_get_bits(bitmap_t* bitmap);

    14. Uhh... Check the math?

    // Gets total number of bytes in bitmap
    size_t bitmap_get_bytes(bitmap_t* bitmap);

    15. Uhh... Check the math?

    // Gets pointer for writing bitmap to file
    uint8_t* bitmap_export(bitmap_t* bitmap);

    16. Uhh... Check the pointer?

    // Creates a new bitmap with the provided data and size
    bitmap_t *bitmap_import(size_t n_bits, uint8_t* bitmap_data);

    17. Import a pattern
    18. 0 n_bits
    19. NULL data pointer

    // Creates a bitmap to contain n bits (zero initialized)
    bitmap *bitmap_initialize(size_t n_bits);

    20. Test normal operation
    21. 0 bit size
    22. Too large data request

    // Destructs and destroys bitmap object
    void bitmap_destroy(bitmap_t* bitmap);

	It's void, so it's a little hard to test...
    23. Uhh... test notmal operation?
    24. ...Test NULL?
*/

bool memcmp_fixed(const uint8_t *const data, uint8_t fixed_value, size_t nbytes) {
    if (data && nbytes) {
        // ... because I'm lazy
        uint8_t *test = (uint8_t *) malloc(nbytes);
        if (test) {
            memset(test, fixed_value, nbytes);
            bool result = memcmp(data, test, nbytes) == 0;
            free(test);
            return result;
        }
    }
    return false;
}

int main() {
    bitmap_t *bitmap_A, *bitmap_B;
    const size_t test_bit_count = 58, test_byte_count = 8;
    // 58 bits = 7.2 bytes

    // INIT/DESTRUCT to get them out of the way
    // 20
    assert(bitmap_A = bitmap_initialize(test_bit_count));
    assert(bitmap_A->data); // Can't do much with this...
    assert(bitmap_A->bit_count == test_bit_count);
    assert(bitmap_A->byte_count == test_byte_count);
    // (also 16)
    assert(bitmap_export(bitmap_A) == bitmap_A->data);
    assert(memcmp_fixed(bitmap_export(bitmap_A), 0x00, test_byte_count));

    // 21
    assert(bitmap_initialize(0) == NULL);

    // 22
    // Should fail wherever we go. Probably. Hmm.
    assert(bitmap_initialize(SIZE_MAX) == NULL);

    // 24
    bitmap_destroy(NULL);
    // 23
    bitmap_destroy(bitmap_A);

    // INIT/DESTROY tested and clear for unchecked use

    bitmap_A = bitmap_initialize(test_bit_count);

    // SET

    assert(memcmp_fixed(bitmap_A->data, 0x00, test_byte_count));
    // 2
    // set bit, check the byte, check the others
    bitmap_set(bitmap_A, 0);
    assert(bitmap_A->data[0] == 0x01);
    assert(memcmp_fixed(bitmap_A->data + 1, 0x00, test_byte_count - 1));
    // 3
    bitmap_set(bitmap_A, test_bit_count - 1);
    assert(bitmap_A->data[test_byte_count - 1] == 0x02);
    assert(memcmp_fixed(bitmap_A->data + 1, 0x00, test_byte_count - 2));
    assert(bitmap_A->data[0] == 0x01);
    // 1
    // set 0-7 on byte 2, check full contents after each set
    bitmap_set(bitmap_A, 8);
    assert(bitmap_A->data[test_byte_count - 1] == 0x02);
    assert(memcmp_fixed(bitmap_A->data + 2, 0x00, test_byte_count - 3));
    assert(bitmap_A->data[0] == 0x01);
    assert(bitmap_A->data[1] == 0x01);

    bitmap_set(bitmap_A, 9);
    assert(bitmap_A->data[test_byte_count - 1] == 0x02);
    assert(memcmp_fixed(bitmap_A->data + 2, 0x00, test_byte_count - 3));
    assert(bitmap_A->data[0] == 0x01);
    assert(bitmap_A->data[1] == 0x03);

    bitmap_set(bitmap_A, 10);
    assert(bitmap_A->data[test_byte_count - 1] == 0x02);
    assert(memcmp_fixed(bitmap_A->data + 2, 0x00, test_byte_count - 3));
    assert(bitmap_A->data[0] == 0x01);
    assert(bitmap_A->data[1] == 0x07);

    bitmap_set(bitmap_A, 11);
    assert(bitmap_A->data[test_byte_count - 1] == 0x02);
    assert(memcmp_fixed(bitmap_A->data + 2, 0x00, test_byte_count - 3));
    assert(bitmap_A->data[0] == 0x01);
    assert(bitmap_A->data[1] == 0x0F);

    bitmap_set(bitmap_A, 12);
    assert(bitmap_A->data[test_byte_count - 1] == 0x02);
    assert(memcmp_fixed(bitmap_A->data + 2, 0x00, test_byte_count - 3));
    assert(bitmap_A->data[0] == 0x01);
    assert(bitmap_A->data[1] == 0x1F);

    bitmap_set(bitmap_A, 13);
    assert(bitmap_A->data[test_byte_count - 1] == 0x02);
    assert(memcmp_fixed(bitmap_A->data + 2, 0x00, test_byte_count - 3));
    assert(bitmap_A->data[0] == 0x01);
    assert(bitmap_A->data[1] == 0x3F);

    bitmap_set(bitmap_A, 14);
    assert(bitmap_A->data[test_byte_count - 1] == 0x02);
    assert(memcmp_fixed(bitmap_A->data + 2, 0x00, test_byte_count - 3));
    assert(bitmap_A->data[0] == 0x01);
    assert(bitmap_A->data[1] == 0x7F);

    bitmap_set(bitmap_A, 15);
    assert(bitmap_A->data[test_byte_count - 1] == 0x02);
    assert(memcmp_fixed(bitmap_A->data + 2, 0x00, test_byte_count - 3));
    assert(bitmap_A->data[0] == 0x01);
    assert(bitmap_A->data[1] == 0xFF);

    // SET tested and clear for use

    // CLEAR
    // same as set, but clearing them and backwards
    // and throwing in the AT/TEST tests (7, 8, 9)

    // 4 & 7
    assert(bitmap_at(bitmap_A, 15));
    assert(bitmap_test(bitmap_A, 15));
    bitmap_reset(bitmap_A, 15);
    assert(bitmap_A->data[test_byte_count - 1] == 0x02);
    assert(memcmp_fixed(bitmap_A->data + 2, 0x00, test_byte_count - 3));
    assert(bitmap_A->data[0] == 0x01);
    assert(bitmap_A->data[1] == 0x7F);
    assert(bitmap_at(bitmap_A, 15) == false);
    assert(bitmap_test(bitmap_A, 15) == false);

    assert(bitmap_at(bitmap_A, 14));
    assert(bitmap_test(bitmap_A, 14));
    bitmap_reset(bitmap_A, 14);
    assert(bitmap_A->data[test_byte_count - 1] == 0x02);
    assert(memcmp_fixed(bitmap_A->data + 2, 0x00, test_byte_count - 3));
    assert(bitmap_A->data[0] == 0x01);
    assert(bitmap_A->data[1] == 0x3F);
    assert(bitmap_at(bitmap_A, 14) == false);
    assert(bitmap_test(bitmap_A, 14) == false);

    assert(bitmap_at(bitmap_A, 13));
    assert(bitmap_test(bitmap_A, 13));
    bitmap_reset(bitmap_A, 13);
    assert(bitmap_A->data[test_byte_count - 1] == 0x02);
    assert(memcmp_fixed(bitmap_A->data + 2, 0x00, test_byte_count - 3));
    assert(bitmap_A->data[0] == 0x01);
    assert(bitmap_A->data[1] == 0x1F);
    assert(bitmap_at(bitmap_A, 13) == false);
    assert(bitmap_test(bitmap_A, 13) == false);

    assert(bitmap_at(bitmap_A, 12));
    assert(bitmap_test(bitmap_A, 12));
    bitmap_reset(bitmap_A, 12);
    assert(bitmap_A->data[test_byte_count - 1] == 0x02);
    assert(memcmp_fixed(bitmap_A->data + 2, 0x00, test_byte_count - 3));
    assert(bitmap_A->data[0] == 0x01);
    assert(bitmap_A->data[1] == 0x0F);
    assert(bitmap_at(bitmap_A, 12) == false);
    assert(bitmap_test(bitmap_A, 12) == false);

    assert(bitmap_at(bitmap_A, 11));
    assert(bitmap_test(bitmap_A, 11));
    bitmap_reset(bitmap_A, 11);
    assert(bitmap_A->data[test_byte_count - 1] == 0x02);
    assert(memcmp_fixed(bitmap_A->data + 2, 0x00, test_byte_count - 3));
    assert(bitmap_A->data[0] == 0x01);
    assert(bitmap_A->data[1] == 0x07);
    assert(bitmap_at(bitmap_A, 11) == false);
    assert(bitmap_test(bitmap_A, 11) == false);

    assert(bitmap_at(bitmap_A, 10));
    assert(bitmap_test(bitmap_A, 10));
    bitmap_reset(bitmap_A, 10);
    assert(bitmap_A->data[test_byte_count - 1] == 0x02);
    assert(memcmp_fixed(bitmap_A->data + 2, 0x00, test_byte_count - 3));
    assert(bitmap_A->data[0] == 0x01);
    assert(bitmap_A->data[1] == 0x03);
    assert(bitmap_at(bitmap_A, 10) == false);
    assert(bitmap_test(bitmap_A, 10) == false);

    assert(bitmap_at(bitmap_A, 9));
    assert(bitmap_test(bitmap_A, 9));
    bitmap_reset(bitmap_A, 9);
    assert(bitmap_A->data[test_byte_count - 1] == 0x02);
    assert(memcmp_fixed(bitmap_A->data + 2, 0x00, test_byte_count - 3));
    assert(bitmap_A->data[0] == 0x01);
    assert(bitmap_A->data[1] == 0x01);
    assert(bitmap_at(bitmap_A, 9) == false);
    assert(bitmap_test(bitmap_A, 9) == false);

    assert(bitmap_at(bitmap_A, 8));
    assert(bitmap_test(bitmap_A, 8));
    bitmap_reset(bitmap_A, 8);
    assert(bitmap_A->data[test_byte_count - 1] == 0x02);
    assert(memcmp_fixed(bitmap_A->data + 1, 0x00, test_byte_count - 2));
    assert(bitmap_A->data[0] == 0x01);
    assert(bitmap_at(bitmap_A, 8) == false);
    assert(bitmap_test(bitmap_A, 8) == false);

    // 6 & 9
    assert(bitmap_at(bitmap_A, test_bit_count - 1));
    assert(bitmap_test(bitmap_A, test_bit_count - 1));
    bitmap_reset(bitmap_A, test_bit_count - 1);
    assert(bitmap_A->data[0] == 0x01);
    assert(memcmp_fixed(bitmap_A->data + 1, 0x00, test_byte_count - 1));
    assert(bitmap_at(bitmap_A, test_bit_count - 1) == false);
    assert(bitmap_test(bitmap_A, test_bit_count - 1) == false);

    // 5 & 8
    assert(bitmap_at(bitmap_A, 0));
    assert(bitmap_test(bitmap_A, 0));
    bitmap_reset(bitmap_A, 0);
    assert(memcmp_fixed(bitmap_A->data, 0x00, test_byte_count));
    assert(bitmap_at(bitmap_A, 0) == false);
    assert(bitmap_test(bitmap_A, 0) == false);

    // AT/TEST and RESET tested and clear for unchecked use

    // Redo the last two blocks but with flip instead of SET/RESET
    // 11
    bitmap_flip(bitmap_A, 0);
    assert(bitmap_A->data[0] == 0x01);
    assert(memcmp_fixed(bitmap_A->data + 1, 0x00, test_byte_count - 1));
    // 12
    bitmap_flip(bitmap_A, test_bit_count - 1);
    assert(bitmap_A->data[test_byte_count - 1] == 0x02);
    assert(memcmp_fixed(bitmap_A->data + 1, 0x00, test_byte_count - 2));
    assert(bitmap_A->data[0] == 0x01);
    // 10
    bitmap_flip(bitmap_A, 8);
    assert(bitmap_A->data[test_byte_count - 1] == 0x02);
    assert(memcmp_fixed(bitmap_A->data + 2, 0x00, test_byte_count - 3));
    assert(bitmap_A->data[0] == 0x01);
    assert(bitmap_A->data[1] == 0x01);

    bitmap_flip(bitmap_A, 9);
    assert(bitmap_A->data[test_byte_count - 1] == 0x02);
    assert(memcmp_fixed(bitmap_A->data + 2, 0x00, test_byte_count - 3));
    assert(bitmap_A->data[0] == 0x01);
    assert(bitmap_A->data[1] == 0x03);

    bitmap_flip(bitmap_A, 10);
    assert(bitmap_A->data[test_byte_count - 1] == 0x02);
    assert(memcmp_fixed(bitmap_A->data + 2, 0x00, test_byte_count - 3));
    assert(bitmap_A->data[0] == 0x01);
    assert(bitmap_A->data[1] == 0x07);

    bitmap_flip(bitmap_A, 11);
    assert(bitmap_A->data[test_byte_count - 1] == 0x02);
    assert(memcmp_fixed(bitmap_A->data + 2, 0x00, test_byte_count - 3));
    assert(bitmap_A->data[0] == 0x01);
    assert(bitmap_A->data[1] == 0x0F);

    bitmap_flip(bitmap_A, 12);
    assert(bitmap_A->data[test_byte_count - 1] == 0x02);
    assert(memcmp_fixed(bitmap_A->data + 2, 0x00, test_byte_count - 3));
    assert(bitmap_A->data[0] == 0x01);
    assert(bitmap_A->data[1] == 0x1F);

    bitmap_flip(bitmap_A, 13);
    assert(bitmap_A->data[test_byte_count - 1] == 0x02);
    assert(memcmp_fixed(bitmap_A->data + 2, 0x00, test_byte_count - 3));
    assert(bitmap_A->data[0] == 0x01);
    assert(bitmap_A->data[1] == 0x3F);

    bitmap_flip(bitmap_A, 14);
    assert(bitmap_A->data[test_byte_count - 1] == 0x02);
    assert(memcmp_fixed(bitmap_A->data + 2, 0x00, test_byte_count - 3));
    assert(bitmap_A->data[0] == 0x01);
    assert(bitmap_A->data[1] == 0x7F);

    bitmap_flip(bitmap_A, 15);
    assert(bitmap_A->data[test_byte_count - 1] == 0x02);
    assert(memcmp_fixed(bitmap_A->data + 2, 0x00, test_byte_count - 3));
    assert(bitmap_A->data[0] == 0x01);
    assert(bitmap_A->data[1] == 0xFF);

    // FLIP BACK

    // 10
    assert(bitmap_at(bitmap_A, 15));
    assert(bitmap_test(bitmap_A, 15));
    bitmap_flip(bitmap_A, 15);
    assert(bitmap_A->data[test_byte_count - 1] == 0x02);
    assert(memcmp_fixed(bitmap_A->data + 2, 0x00, test_byte_count - 3));
    assert(bitmap_A->data[0] == 0x01);
    assert(bitmap_A->data[1] == 0x7F);
    assert(bitmap_at(bitmap_A, 15) == false);
    assert(bitmap_test(bitmap_A, 15) == false);

    assert(bitmap_at(bitmap_A, 14));
    assert(bitmap_test(bitmap_A, 14));
    bitmap_flip(bitmap_A, 14);
    assert(bitmap_A->data[test_byte_count - 1] == 0x02);
    assert(memcmp_fixed(bitmap_A->data + 2, 0x00, test_byte_count - 3));
    assert(bitmap_A->data[0] == 0x01);
    assert(bitmap_A->data[1] == 0x3F);
    assert(bitmap_at(bitmap_A, 14) == false);
    assert(bitmap_test(bitmap_A, 14) == false);

    assert(bitmap_at(bitmap_A, 13));
    assert(bitmap_test(bitmap_A, 13));
    bitmap_flip(bitmap_A, 13);
    assert(bitmap_A->data[test_byte_count - 1] == 0x02);
    assert(memcmp_fixed(bitmap_A->data + 2, 0x00, test_byte_count - 3));
    assert(bitmap_A->data[0] == 0x01);
    assert(bitmap_A->data[1] == 0x1F);
    assert(bitmap_at(bitmap_A, 13) == false);
    assert(bitmap_test(bitmap_A, 13) == false);

    assert(bitmap_at(bitmap_A, 12));
    assert(bitmap_test(bitmap_A, 12));
    bitmap_flip(bitmap_A, 12);
    assert(bitmap_A->data[test_byte_count - 1] == 0x02);
    assert(memcmp_fixed(bitmap_A->data + 2, 0x00, test_byte_count - 3));
    assert(bitmap_A->data[0] == 0x01);
    assert(bitmap_A->data[1] == 0x0F);
    assert(bitmap_at(bitmap_A, 12) == false);
    assert(bitmap_test(bitmap_A, 12) == false);

    assert(bitmap_at(bitmap_A, 11));
    assert(bitmap_test(bitmap_A, 11));
    bitmap_flip(bitmap_A, 11);
    assert(bitmap_A->data[test_byte_count - 1] == 0x02);
    assert(memcmp_fixed(bitmap_A->data + 2, 0x00, test_byte_count - 3));
    assert(bitmap_A->data[0] == 0x01);
    assert(bitmap_A->data[1] == 0x07);
    assert(bitmap_at(bitmap_A, 11) == false);
    assert(bitmap_test(bitmap_A, 11) == false);

    assert(bitmap_at(bitmap_A, 10));
    assert(bitmap_test(bitmap_A, 10));
    bitmap_flip(bitmap_A, 10);
    assert(bitmap_A->data[test_byte_count - 1] == 0x02);
    assert(memcmp_fixed(bitmap_A->data + 2, 0x00, test_byte_count - 3));
    assert(bitmap_A->data[0] == 0x01);
    assert(bitmap_A->data[1] == 0x03);
    assert(bitmap_at(bitmap_A, 10) == false);
    assert(bitmap_test(bitmap_A, 10) == false);

    assert(bitmap_at(bitmap_A, 9));
    assert(bitmap_test(bitmap_A, 9));
    bitmap_flip(bitmap_A, 9);
    assert(bitmap_A->data[test_byte_count - 1] == 0x02);
    assert(memcmp_fixed(bitmap_A->data + 2, 0x00, test_byte_count - 3));
    assert(bitmap_A->data[0] == 0x01);
    assert(bitmap_A->data[1] == 0x01);
    assert(bitmap_at(bitmap_A, 9) == false);
    assert(bitmap_test(bitmap_A, 9) == false);

    assert(bitmap_at(bitmap_A, 8));
    assert(bitmap_test(bitmap_A, 8));
    bitmap_flip(bitmap_A, 8);
    assert(bitmap_A->data[test_byte_count - 1] == 0x02);
    assert(memcmp_fixed(bitmap_A->data + 1, 0x00, test_byte_count - 2));
    assert(bitmap_A->data[0] == 0x01);
    assert(bitmap_at(bitmap_A, 8) == false);
    assert(bitmap_test(bitmap_A, 8) == false);

    // 12
    assert(bitmap_at(bitmap_A, test_bit_count - 1));
    assert(bitmap_test(bitmap_A, test_bit_count - 1));
    bitmap_flip(bitmap_A, test_bit_count - 1);
    assert(bitmap_A->data[0] == 0x01);
    assert(memcmp_fixed(bitmap_A->data + 1, 0x00, test_byte_count - 1));
    assert(bitmap_at(bitmap_A, test_bit_count - 1) == false);
    assert(bitmap_test(bitmap_A, test_bit_count - 1) == false);

    // 11
    assert(bitmap_at(bitmap_A, 0));
    assert(bitmap_test(bitmap_A, 0));
    bitmap_flip(bitmap_A, 0);
    assert(memcmp_fixed(bitmap_A->data, 0x00, test_byte_count));
    assert(bitmap_at(bitmap_A, 0) == false);
    assert(bitmap_test(bitmap_A, 0) == false);

    // FLIP tested and clear for unchecked use

    // Test the remaining extras (get, format, etc...)

    // 13
    bitmap_format(bitmap_A, 0xFA);
    assert(memcmp_fixed(bitmap_A->data, 0xFA, test_byte_count));

    // 14
    assert(bitmap_get_bits(bitmap_A) == test_bit_count);

    // 15
    assert(bitmap_get_bytes(bitmap_A) == test_byte_count);

    // 16
    assert(bitmap_export(bitmap_A) == bitmap_A->data);

    // 18
    assert(bitmap_import(0, bitmap_A->data) == NULL);

    // 19
    assert(bitmap_import(test_bit_count, NULL) == NULL);

    // 17
    assert(bitmap_B = bitmap_import(test_bit_count,bitmap_A->data));
    assert(memcmp(bitmap_B->data,bitmap_A->data, test_byte_count) == 0);

    // Done. GO TEAM!

    puts("TESTS PASSED");
}