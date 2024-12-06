#include "../include/soquete-lib.h"

// int recebeResposta(int soquete,unsigned char *buffer);

void menu();

void handle_backup(char *segundo_token,struct sockaddr_ll endereco,int soquete,unsigned char *sequencia,unsigned char *bufferResposta);