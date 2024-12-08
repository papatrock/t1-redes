#ifndef SERVIDOR_BACKUP_H
#define SERVIDOR_BACKUP_H
#include "../include/soquete-lib.h"

int enviaResposta(int soquete, struct sockaddr_ll endereco, protocolo_t resposta);

void extraiMacFonte(unsigned char *packet, unsigned char *src_mac);

void handle_backup(unsigned char* buffer, int soquete, struct sockaddr_ll path_addr,unsigned char sequencia,protocolo_t resposta);

void handle_verifica(unsigned char* mensagem, int soquete, struct sockaddr_ll path_addr,unsigned char sequencia,protocolo_t resposta);

#endif