#include "../include/servidor.h"

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
            //verifica tipo
            unsigned char tipo = getTipo(buffer);
            
            //-----BACKUP------
                if(tipo == 4){
                    printf("ENTROU NO BACKUP\n");
                    
                    // Abre  pasta Backup e abre (ou criar) o  arquivo com o nome solicitado para receber dados
                    printf("%s\n",getDados(buffer));

                    FILE *arq = fopen ((char*)getDados(buffer),"w");
                    if(!arq)
                    {
                        printf("erro ao abrir o arquivo\n");
                        return 1;
                    }
                    printf("ABRIU O ARQUIVO IRRAAAAAAAAAAAAAAA\n");
                    fclose(arq);
                }


            printf("Pacote recebido (%d bytes):\n", bytes_recebidos);
            printMensagem(buffer); 
            extraiMacFonte(buffer, macFonte);
            

    for (int i = 0; i < 63; i++) {
        printf("%02X ", buffer[i]);
    }
    printf("\n");


            printf("MAC de origem: %02x:%02x:%02x:%02x:%02x:%02x\n", 
                   macFonte[0], macFonte[1], macFonte[2], macFonte[3], macFonte[4], macFonte[5]);

            inicializaSockaddr_ll(&dest_addr, ifindex, macFonte);
            enviaResposta(soquete, dest_addr, macFonte);
        }
    }

    close(soquete);
    return 0;
}
