#include "../include/utils.h"

off_t file_size(const char *file_path) {
    struct stat file_stat;

    if(stat(file_path, &file_stat) == 0)
        return file_stat.st_size;

    return -1;
}