#ifndef SOQUETE_LIB_H
#define SOQUETE_LIB_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h> // para close
#include <linux/if_packet.h>
#include <sys/socket.h>
#include <net/ethernet.h>
#include <net/if.h>
#include <arpa/inet.h> // para htons
#include <netinet/in.h> // Para htons
#include <linux/if_ether.h>
#include <netinet/ip.h>
#include <libgen.h>
#include <dirent.h>
#include <sys/types.h>
#include "../include/utils.h"
#include <errno.h> // para erros de timeout




#define INTERFACE "enp2s0"
#define PACOTE 63
#define POLINOMIO_DIVISOR 0b100000111
#define MARCADOR 0b01111110

// tipos de mensagens
#define ACK 0
#define NACK 1
#define OK 2
#define BACKUP 4
#define RESTAURA 5
#define VERIFICA 6
#define OK_CHECKSUM 13
#define OK_TAM 14
#define TAMANHO 15
#define DADOS 16
#define FIM_TRANSMISSAO_DADOS 17
#define ERRO 31

// tipos de erro
#define SEM_PERMISSAO '1'
#define SEM_ESPACO '2'
#define ARQUIVO_NAO_ENCONTRADO '3'

struct protocolo{
    unsigned char marcador; //8 bits
    unsigned char tamanho; //6 bits
    unsigned char sequencia; //5 bits
    unsigned char tipo; //5 bits
    char dados[63]; //0 até 63 bytes
    unsigned char CRC; //8 bits
};

typedef struct protocolo protocolo_t;

int criaSocket(char *interface);

void inicializaSockaddr_ll(struct sockaddr_ll *sockaddr, int ifindex, unsigned char *dest_mac);

protocolo_t criaMensagem(unsigned char tamanho,unsigned char sequencia,unsigned char tipo,char *dados);

int recebeResposta(int soquete,unsigned char *buffer, protocolo_t ultima_mensagem, struct sockaddr_ll endereco,unsigned char *sequencia);

void printMensagem(unsigned char *mensagem);

void printMensagemEstruturada(protocolo_t mensagem);

void printMensagemEstruturadaBinario(protocolo_t mensagem);

unsigned char getMarcador(unsigned char *mensagem);

unsigned char getTamanho(unsigned char *mensagem);

unsigned char getSequencia(unsigned char *mensagem);

unsigned char getTipo(unsigned char *mensagem);

char *getDados(unsigned char *mensagem);

unsigned char getCRC(unsigned char *mensagem);

char *getErrors(unsigned char *errors);

void setErrorMessage(char error_code, char* error_message);

unsigned char geraCRC(unsigned char *ptr);

int verificaCRC(unsigned char *mensagemRecebida);

void printBits(unsigned char byte);

unsigned char *empacota(unsigned char *mensagem);

unsigned char *empacotaStruct(protocolo_t *mensagem);

#endif