#include "../include/servidor.h"
#include <net/ethernet.h>
#include <linux/if_packet.h>
#include <arpa/inet.h>

void enviaResposta(int soquete, struct sockaddr_ll endereco, unsigned char *src_mac) {


	protocolo_t resposta = criaMensagem("ACK",0b00000);

    if (sendto(soquete, &resposta, sizeof(resposta), 0, (struct sockaddr*)&endereco, sizeof(endereco)) == -1) {
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
            printMensagem(buffer); 
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
