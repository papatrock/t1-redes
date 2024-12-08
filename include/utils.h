#include <sys/stat.h>
#include <sys/types.h>
#include <stdlib.h>
#include <stdio.h>

off_t file_size(const char *file_path);

void char_to_binary(unsigned char *string, int size, unsigned char *dest);

void gera_erro(unsigned char *mensagem);

void imprimir_binario(unsigned char *mensagem, size_t tamanho);

void print_byte_as_binary(unsigned char byte, int bits);

char *checksum(const char *nome_arquivo);