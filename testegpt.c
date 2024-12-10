#include <stdio.h>
#include <string.h>

#define POLINOMIO_DIVISOR 0b100000111

void char_to_binary(unsigned char *string, int size, unsigned char *dest) {
    *dest = 0;  // Reset buffer
    for (int i = 0; i < size; ++i) {
        if (string[i] == '1') {
            *dest |= (1 << (size - 1 - i));  // Set bit at correct position
        }
    }
}

int main() {
    unsigned char ptr[] = "01111110000010010000000000000100011101000110010101110011011101000010111001110100011110000111010000000000"; // Original message
    unsigned char buffer = 0;  // Buffer to store bits
    int deslocamento = 0;
    unsigned char crc = 0;

    while (deslocamento <= (int)strlen(ptr) - 9) { // Process while there are enough bits for division
        if (ptr[deslocamento] == '0') {
            deslocamento++;
        } else {
            printf("\n\nPONTEIRO ORIGINAL: %s\n", ptr);
            char_to_binary(ptr + deslocamento, 9, &buffer); // Convert 9 bits to binary
            printf("BUFFER: %u\n", buffer);

            crc = buffer ^ POLINOMIO_DIVISOR; // XOR with the divisor
            printf("CRC do deslocamento %d = %u\n", deslocamento, crc);

            // Update ptr with the XOR result
            for (int j = 0; j < 9; j++) {
                unsigned int mask = 1 << (8 - j); // Mask for current bit
                ptr[deslocamento + j] = (crc & mask) ? '1' : '0'; // Update with '0' or '1'
            }
            deslocamento++; // Move to the next bit
        }
    }

    // Print final message and CRC
    printf("Final ptr: %s\n", ptr);
    printf("CRC: %u\n", crc);

    return 0;
}
