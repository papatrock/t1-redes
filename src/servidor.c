#include "../include/servidor-restaura.h"
#include "../include/servidor.h"
#include "../include/utils.h"
#include <sys/time.h>


struct timeval timeout;
struct timeval no_timeout = {0};

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
    timeout.tv_sec = 0;
    timeout.tv_usec=500000;
    unsigned char macFonte[6];
    struct sockaddr_ll endereco;
    int ifindex = if_nametoindex(INTERFACE);
    unsigned char sequencia = 0;
    
    int soquete = criaSocket(INTERFACE);

    if (ifindex == 0) {
        fprintf(stderr, "Erro: Interface não encontrada\n");
        return -1;
    }

    #ifdef _SIMULA_ERRO_
        int qtd_erro = 1;      
    #endif

    int timeout_ativado = 0;

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

            // ativa o to
            if (!timeout_ativado) {
                    timeout.tv_sec = 0;
                    timeout.tv_usec = 500000; // 500ms

                    if (setsockopt(soquete, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout)) < 0) {
                        perror("Erro ao configurar timeout de recepção");
                        return 1;
                    }

                    timeout_ativado = 1; 
            }

            //Coleta endereço do cliente
            extraiMacFonte(buffer, macFonte);
            inicializaSockaddr_ll(&endereco, ifindex, macFonte);
            #ifdef _DEBUG_
            printf("MAC de origem: %02x:%02x:%02x:%02x:%02x:%02x\n", 
                   macFonte[0], macFonte[1], macFonte[2], macFonte[3], macFonte[4], macFonte[5]);
            #endif /* ifdef _DEBUG_ */

            #ifdef _SIMULA_ERRO_
            if(qtd_erro > 0){
                gera_erro(buffer);
                printf("gerou um erro\n");
            }
            qtd_erro--;
            #endif


            // Verifica CRC, se 0 envia um nack
            if(!verificaCRC(buffer)){
                #ifdef _DEBUG_
                printf("entrou no CRC =! 0\n");
                #endif
                resposta = criaMensagem(0,sequencia,NACK,"");
                enviaResposta(soquete,endereco,resposta);
                continue;
            }

// ----------------------------- MENSAGEM VALIDA ----------------------------------------
            //verifica tipo
            switch (getTipo(buffer))
            {
            case BACKUP:  
                handle_backup(buffer, soquete, endereco,sequencia);
                break;
            case RESTAURA:
                handle_restaura(buffer, soquete, endereco,&sequencia);
                break;
            
            case VERIFICA:
                handle_verifica(buffer, soquete, endereco,sequencia);
                break;

                default:
            }

        }

        // desativa o timeout
        if (setsockopt(soquete, SOL_SOCKET, SO_RCVTIMEO, &no_timeout, sizeof(no_timeout)) < 0) {
            perror("Erro ao desativar timeout");
            return 1;
        }
        timeout_ativado = 0; // Marca que o timeout foi desativado
        //limpa o buffer
        memset(buffer, 0, sizeof(buffer));
    }

    close(soquete);
    return 0;
}
