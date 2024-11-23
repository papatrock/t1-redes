#include "../include/servidor-restaura.h"
#include "../include/servidor.h"
#include "../include/utils.h"

/**
 * Envia uma mensagem para o mac fonte
 *
 * @param soquete 
 * @param endereco
 * @param resposta
 * @return 1 se sucesso, 0 se fracasso
 */
int enviaResposta(int soquete, struct sockaddr_ll endereco, protocolo_t resposta) {

    if (sendto(soquete, &resposta, sizeof(resposta), 0, (struct sockaddr*)&endereco, sizeof(endereco)) == -1) 
        return 0;
    return 1;
}


void extraiMacFonte(unsigned char *packet, unsigned char *src_mac) {
    // No cabeçalho Ethernet, o MAC de origem começa no byte 6
    memcpy(src_mac, packet + 6, 6);
}



int main() {
    unsigned char macFonte[6];
    struct sockaddr_ll path_addr;
    int ifindex = if_nametoindex(INTERFACE);
    unsigned char sequencia = 0;
    
    int soquete = criaSocket(INTERFACE);

    if (ifindex == 0) {
        fprintf(stderr, "Erro: Interface não encontrada\n");
        return -1;
    }

    while (1) {
        unsigned char buffer[68];
        struct sockaddr_ll addr;
        socklen_t addr_len = sizeof(addr);
        protocolo_t resposta;
        int bytes_recebidos = recvfrom(soquete, buffer, sizeof(buffer), 0, (struct sockaddr*)&addr, &addr_len);


        if (bytes_recebidos == -1) {
            perror("Erro ao receber dados");
            continue;
        } else {
            if (bytes_recebidos > 0 && buffer[0] != 0b01111110)
                continue;
            
            #ifdef _DEBUG_
            printf("Recebeu uma mensagem:\n");
            printMensagem(buffer);
            #endif

            //Coleta endereço do cliente
            extraiMacFonte(buffer, macFonte);
            inicializaSockaddr_ll(&path_addr, ifindex, macFonte);
            #ifdef _DEBUG_
            
            printf("MAC de origem: %02x:%02x:%02x:%02x:%02x:%02x\n", 
                   macFonte[0], macFonte[1], macFonte[2], macFonte[3], macFonte[4], macFonte[5]);
            #endif /* ifdef _DEBUG_ */
            //verifica tipo
            unsigned char tipo = getTipo(buffer);
            
            //-----BACKUP------
            if(tipo == BACKUP){
                #ifdef _DEBUG_ 
                printf("ENTROU NO BACKUP\n");
                
                #endif
                
                handle_backup(buffer, soquete, path_addr,sequencia,resposta);
            }
            //-----RESTAURA------
            else if(tipo == RESTAURA) {
                printf("chamou restaura\n");
                handle_restaura(buffer, soquete, path_addr);
            }
        }
    }

    close(soquete);
    return 0;
}
