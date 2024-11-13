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
                if(tipo == 4){
                    #ifdef _DEBUG_ 
                    printf("ENTROU NO BACKUP\n");
                   
                    #endif
                    
                    // Abre  pasta Backup e abre (ou criar) o  arquivo com o nome solicitado para receber dados
                    char path[100]; 
                    strcpy(path, "Backup/"); 
                    strcat(path, (char*)getDados(buffer)); 

                    FILE *arq = fopen (path,"wb+");
                    if(!arq)
                    {
                        printf("erro ao abrir o arquivo, enviando nack\n");
                        //Manda um nack
                        resposta = criaMensagem(0,0,1,"Erro ao abrir arquivo",0);
                        if(!enviaResposta(soquete,path_addr,resposta))
                            printf("Erro ao enviar resposta\n");
                        else
                            printf("Resposta enviada com sucesso\n");                          
                    }
                    else{
                        #ifdef _DEBUG_
                        printf("Abriu arquivo\n"); 
                        #endif /* ifdef  */
                        //Manda um ok e aguarda o tamanho
                        resposta = criaMensagem(0,0,2,"Ok!",0);
                        if(!enviaResposta(soquete,path_addr,resposta))
                                printf("Erro ao enviar resposta\n");
                        else
                            printf("Resposta enviada com sucesso, aguardando tamanho\n");

                        recebeResposta(soquete,buffer);
                        //TODO verificar se cabe em disco
                        printf("recebeu dados:\n");
                        printMensagem(buffer);
                        // SE COUBER:
                        
                        resposta = criaMensagem(0,0,2,"Ok!",0);
                        
                        if(!enviaResposta(soquete,path_addr,resposta))
                                printf("Erro ao enviar resposta\n");
                        else
                            printf("Resposta enviada com sucesso, aguardando dados\n");
                        

                        //TODO tratar erros aqui
                        //RECEBENDO DADOS  
                        while (getTipo(buffer) != 17){
                            recebeResposta(soquete,buffer);
                            //dados
                            if(getTipo(buffer) == 16){
                                #ifdef _DEBUG_ 
                                printf("Recebeu um pacote de dados:\n");
                                printMensagem(buffer);
                                #endif
                                char *dados = (char*)getDados(buffer);
                                fwrite(dados,strlen(dados),1,arq);
                                fflush(arq);
                            }
                                
                        }
                        printf("Terminou de receber dados\n");
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
