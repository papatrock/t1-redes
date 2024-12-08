#include <sys/stat.h>
#include <sys/types.h>

off_t file_size(const char *file_path);

void char_to_binary(unsigned char *string, int size, unsigned char *dest);

void gera_erro(unsigned char *mensagem);
