#include "../include/soquete-lib.h"
#include <unistd.h> // Para usleep (no Windows, use <windows.h> e Sleep)
#include <pthread.h> 

// int recebeResposta(int soquete,unsigned char *buffer);

void menu();

void handle_backup(char *segundo_token,struct sockaddr_ll endereco,int soquete,unsigned char *sequencia,unsigned char *bufferResposta);

void handle_verifica(char* nome_arq, struct sockaddr_ll endereco, int soquete, unsigned char *sequencia, unsigned char *bufferResposta);