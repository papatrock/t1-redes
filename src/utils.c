#include "../include/utils.h"

off_t file_size(const char *file_path) {
    struct stat file_stat;

    if(stat(file_path, &file_stat) == 0)
        return file_stat.st_size;

    return -1;
}

void char_to_binary(unsigned char *string, int size, unsigned char *dest) {
    *dest = 0;  // Reset buffer (dest)
    for (int i = 0; i < size; ++i) {
        if (string[i] == '1') {
            *dest = *dest | (1 << (size - 1 - i));  // Set bit at correct position
        }
    }
}