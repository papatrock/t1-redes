#include "../include/soquete-lib.h"


void enviaResposta(int soquete, struct sockaddr_ll endereco, unsigned char *src_mac);


void extraiMacFonte(unsigned char *packet, unsigned char *src_mac);


void inicializaSockaddr_ll(struct sockaddr_ll *sockaddr, int ifindex, unsigned char *dest_mac);


