#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h> // para close
#include <linux/if_packet.h>
#include <sys/socket.h>
#include <net/if.h>
#include <arpa/inet.h> // para htons
#include <netinet/in.h> // Para htons
#include <linux/if_ether.h>
#include <netinet/ip.h>

#define INTERFACE "lo"
#define MEU_PROTOCOLO 0x88b5

struct protocolo{
    unsigned int marcador : 8;
    unsigned int tamanho : 6;
    unsigned int sequencia : 5;
    unsigned int tipo : 5;
    unsigned char dados[63];
    unsigned int CRC : 8;
};

typedef struct protocolo protocolo_t;


int criaSocket(char *interface);
