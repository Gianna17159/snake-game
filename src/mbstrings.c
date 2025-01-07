#include "mbstrings.h"

#include <stddef.h>

/* mbslen - multi-byte string length
 * - Description: returns the number of UTF-8 code points ("characters")
 * in a multibyte string. If the argument is NULL or an invalid UTF-8
 * string is passed, returns -1.
 *
 * - Arguments: A pointer to a character array (`bytes`), consisting of UTF-8
 * variable-length encoded multibyte code points.
 *
 * - Return: returns the actual number of UTF-8 code points in `src`. If an
 * invalid sequence of bytes is encountered, return -1.
 *
 */
size_t mbslen(const char* bytes) {
    int index = 0;
    int num_characters = 0;
    unsigned char c = (unsigned char)bytes[index];
    unsigned char bits = 240;
    while (c != '\0') {
        int skip = 1;
        unsigned char c_and_bits = c & bits;
        unsigned char first = c_and_bits >> 7;
        unsigned char first_2 = c_and_bits >> 6;
        unsigned char first_3 = c_and_bits >> 5;
        unsigned char first_4 = c_and_bits >> 4;

        if (first_4 == 15) {
            skip = 4;
        } else if (first_3 == 7) {
            skip = 3;
        } else if (first_2 == 3) {
            skip = 2;
        } else if (first == 0) {
            skip = 1;
        }
        index += skip;
        num_characters++;
        c = (unsigned char)bytes[index];
    }
    return num_characters;
}
