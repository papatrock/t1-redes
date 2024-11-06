#include "../include/soquete-lib.h"

int criaSocket(char *interface)
{
    int soquete = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ALL));
    
    if(soquete == -1)
    {
        fprintf(stderr,"Erro ao criar socket, verifique se você é root\n");
        exit(-1);
    }
    
    int ifindex = if_nametoindex(interface);

    struct sockaddr_ll endereco = {0};
    endereco.sll_family = AF_PACKET;
    endereco.sll_protocol = htons(ETH_P_ALL);
    endereco.sll_ifindex = ifindex;

    if(bind(soquete, (struct sockaddr*) &endereco, sizeof(endereco)) == -1)
    {
        fprintf(stderr,"Erro ao fazer bind no socket\n");
        exit(-1);
    }
    
    struct packet_mreq mr = {0};
    mr.mr_ifindex = ifindex;
    mr.mr_type = PACKET_MR_PROMISC;

    if(setsockopt(soquete, SOL_PACKET, PACKET_ADD_MEMBERSHIP, &mr, sizeof(mr)) == -1) {
        fprintf(stderr,"Erro ao fazer setsockopt: Verifique se a interface de rede foi especificada corretamente\ndefina a interface no soquete-lib.h\n");
        exit(-1);
    }
    return soquete;
}

void inicializaSockaddr_ll(struct sockaddr_ll *sockaddr, int ifindex, unsigned char *dest_mac) {
    memset(sockaddr, 0, sizeof(struct sockaddr_ll));
    sockaddr->sll_family = AF_PACKET;
    sockaddr->sll_protocol = htons(ETH_P_IP);
    sockaddr->sll_ifindex = ifindex;
    if(dest_mac != 0){
        sockaddr->sll_halen = ETH_ALEN;
        memcpy(sockaddr->sll_addr, dest_mac, 6); // Endereço MAC de destino
    }
}

protocolo_t criaMensagem(char *dados, unsigned int tipo) {
    protocolo_t mensagem; 


    mensagem.marcador = 0b01111110;
    mensagem.tamanho = 0b000000;
    mensagem.sequencia = 0b00000;
    mensagem.tipo = tipo;
    strncpy((char *)mensagem.dados, dados, sizeof(mensagem.dados) - 1);
    mensagem.CRC = 0b00000000;

    return mensagem;
}

void print_byte_as_binary(unsigned char byte) {
    for (int i = 7; i >= 0; i--) {
        printf("%d", (byte >> i) & 1);
    }
}

void printMensagem(unsigned char *mensagem) {
    printf("Marcador: ");
    print_byte_as_binary(mensagem[0]);
    printf("\n");

    printf("Tamanho: ");
    for (int i = 7; i >= 2; i--) {
        printf("%d", (mensagem[1] >> i) & 1);
    }
    printf("\n");

    printf("Sequência: ");
    for (int i = 7; i >= 3; i--) {
        printf("%d", (mensagem[2] >> i) & 1);
    }
    printf("\n");

    /*printf("Dados: ");
    for (int i = 3; i < 66; i++) {
        print_byte_as_binary(mensagem[i]);
        printf(" ");
    }
    printf("\n");*/

    printf("Dados (ASCII): ");
    for (int i = 3; i < 63; i++) {
        printf("%c", mensagem[i]);
    }
    printf("\n");

    printf("CRC: ");
    print_byte_as_binary(mensagem[66]);
    printf("\n");
}
