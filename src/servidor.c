#include "../include/servidor.h"

/**
 * Envia uma mensagem para o mac fonte
 *
 * @param soquete 
 * @param endereco
 * @param resposta
 * @return 1 se sucesso, 0 se fracasso
 */
int enviaResposta(int soquete, struct sockaddr_ll endereco, protocolo_t resposta) {

    if (sendto(soquete, &resposta, sizeof(resposta), 0, (struct sockaddr*)&endereco, sizeof(endereco)) == -1) {
        return 0;
    } else {
        return 1;
    }
}


void extraiMacFonte(unsigned char *packet, unsigned char *src_mac) {
    // No cabeçalho Ethernet, o MAC de origem começa no byte 6
    memcpy(src_mac, packet + 6, 6);
}



int main() {
    unsigned char macFonte[6];
    struct sockaddr_ll path_addr;
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

            //Mensagem recebida:
            for (int i = 0; i < 63; i++) {
                printf("%02X ", buffer[i]);
            }
            printf("\n");
            printMensagem(buffer);

            //Coleta endereço do cliente
            extraiMacFonte(buffer, macFonte);
            inicializaSockaddr_ll(&path_addr, ifindex, macFonte);
            printf("MAC de origem: %02x:%02x:%02x:%02x:%02x:%02x\n", 
                   macFonte[0], macFonte[1], macFonte[2], macFonte[3], macFonte[4], macFonte[5]);

            //verifica tipo
            unsigned char tipo = getTipo(buffer);
            
            //-----BACKUP------
                if(tipo == 4){
                    printf("ENTROU NO BACKUP\n");
                    
                    // Abre  pasta Backup e abre (ou criar) o  arquivo com o nome solicitado para receber dados
                    char path[100]; 
                    strcpy(path, "Backup/"); 
                    strcat(path, (char*)getDados(buffer)); 

                    FILE *arq = fopen (path,"r");
                    if(!arq)
                    {
                        printf("erro ao abrir o arquivo, enviando nack\n");
                        //Manda um nack
                        protocolo_t resposta = criaMensagem(0,0,1,"Erro ao abrir arquivo",0);
                        if(!enviaResposta(soquete,path_addr,resposta))
                            printf("Erro ao enviar resposta\n");
                        else
                            printf("Resposta enviada com sucesso\n");
                    }
                    else{
                        printf("ABRIU O ARQUIVO IRRAAAAAAAAAAAAAAA\n");
                        fclose(arq);
                    }
                }

            //protocolo_t resposta = criaMensagem(0,0,0,"Reposta teste",0);
            //enviaResposta(soquete, path_addr, resposta);
        }
    }

    close(soquete);
    return 0;
}
