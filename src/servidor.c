#include "../include/servidor.h"
#include <net/ethernet.h>
#include <linux/if_packet.h>
#include <arpa/inet.h>

void enviaResposta(int soquete, struct sockaddr_ll endereco, unsigned char *src_mac) {
    unsigned char resposta[ETH_HLEN + 3]; // Cabeçalho Ethernet (14 bytes) + "ACK" (3 bytes)

    memcpy(resposta, endereco.sll_addr, 6);      // MAC de destino
    memcpy(resposta + 6, src_mac, 6);            // MAC de origem
    resposta[12] = 0x08;                         // Ethertype (IPv4)
    resposta[13] = 0x00;

    resposta[14] = 'A';
    resposta[15] = 'C';
    resposta[16] = 'K';

    if (sendto(soquete, resposta, sizeof(resposta), 0, (struct sockaddr*)&endereco, sizeof(endereco)) == -1) {
        perror("Erro ao enviar resposta");
    } else {
        printf("Resposta enviada com sucesso\n");
    }
}


void extraiMacFonte(unsigned char *packet, unsigned char *src_mac) {
    // No cabeçalho Ethernet, o MAC de origem começa no byte 6
    memcpy(src_mac, packet + 6, 6);
}



int main() {
    unsigned char macFonte[6];
    struct sockaddr_ll dest_addr;
    int ifindex = if_nametoindex(INTERFACE);
    
    int soquete = criaSocket(INTERFACE);

    if (ifindex == 0) {
        fprintf(stderr, "Erro: Interface não encontrada\n");
        return -1;
    }

    while (1) {
        unsigned char buffer[68];
        struct sockaddr_ll addr;
        socklen_t addr_len = sizeof(addr);
         
        int bytes_recebidos = recvfrom(soquete, buffer, sizeof(buffer), 0, (struct sockaddr*)&addr, &addr_len);
        
        if (bytes_recebidos == -1) {
            perror("Erro ao receber dados");
            continue;
        } else {
            if (bytes_recebidos > 0 && buffer[0] != 0b01111110)
                continue;

            printf("Pacote recebido (%d bytes):\n", bytes_recebidos);
            for (ssize_t i = 0; i < bytes_recebidos; i++) {
                printf("%02x ", (unsigned char)buffer[i]);
                if ((i + 1) % 16 == 0) printf("\n");
            }
            extraiMacFonte(buffer, macFonte);
            
            printf("MAC de origem: %02x:%02x:%02x:%02x:%02x:%02x\n", 
                   macFonte[0], macFonte[1], macFonte[2], macFonte[3], macFonte[4], macFonte[5]);

            inicializaSockaddr_ll(&dest_addr, ifindex, macFonte);
            enviaResposta(soquete, dest_addr, macFonte);
        }
    }

    close(soquete);
    return 0;
}
