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




#define INTERFACE "enp2s0"
#define PACOTE 63

struct protocolo{
    unsigned char marcador; //8 bits
    unsigned char tamanho; //6 bits
    unsigned char sequencia; //5 bits
    unsigned char tipo; //5 bits
    unsigned char dados[63];
    unsigned char CRC; //8 bits
};

typedef struct protocolo protocolo_t;

int criaSocket(char *interface);

void inicializaSockaddr_ll(struct sockaddr_ll *sockaddr, int ifindex, unsigned char *dest_mac);


protocolo_t criaMensagem(unsigned char tamanho,unsigned char sequencia,unsigned char tipo,char *dados,unsigned char CRC);

int recebeResposta(int soquete,unsigned char *buffer);

void print_byte_as_binary(unsigned char byte, int bits);

void printMensagem(unsigned char *mensagem);

void printMensagemEstruturada(protocolo_t mensagem);

unsigned char getMarcador(unsigned char *mensagem);

unsigned char getTamanho(unsigned char *mensagem);

unsigned char getSequencia(unsigned char *mensagem);

unsigned char getTipo(unsigned char *mensagem);

unsigned char *getDados(unsigned char *mensagem);

