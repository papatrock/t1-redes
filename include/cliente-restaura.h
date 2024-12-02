#include "soquete-lib.h"

#define MAX_FILE_VERSION 10
#define MEMORY_ALLOCATION_FAILURE -1
#define MAX_VERSIONS_REACHED -2

int get_next_file_version(char* path, char **path_new_file);

void handle_restaura(char* nome_arq, struct sockaddr_ll endereco, int soquete, unsigned char *sequencia, unsigned char *bufferResposta);
