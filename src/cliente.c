#include "../include/cliente.h"


void menu(){
    printf("\n---------------------\nopções disponiveis:\nbackup <nome do arquivo>\nrestaura <nome do arquivo>\nverifica <nome do arquivo>\nsair\n-----------------\n");
}

int main(int argc, char *argv[]){
    
    unsigned char sequencia,CRC;
    sequencia = CRC = 0;

    int soquete = criaSocket(INTERFACE); 
    int ifindex = if_nametoindex(INTERFACE);
    struct sockaddr_ll endereco;

    unsigned char *bufferResposta = (unsigned char *)malloc(68 * sizeof(unsigned char));
    if (!bufferResposta) {
        printf("erro ao alocar buffer\n");
        return -1;
    }
    
    inicializaSockaddr_ll(&endereco,ifindex,0);
    
    char *entrada = malloc(63 * sizeof(char));
    if (!entrada) {
        fprintf(stderr, "erro ao alocar memória\n");
        return -1;
    }
    menu();
        
    fgets(entrada, 63, stdin);

    // Remove o caractere de nova linha ('\n') que fgets adiciona
    entrada[strcspn(entrada, "\n")] = '\0';

    while (strcmp(entrada, "sair") != 0) {
        
        // Separar a entrada em dois tokens usando espaço como delimitador
        char entrada_copy[100];
        strcpy(entrada_copy,entrada);

        char *primeiro_token = strtok(entrada_copy, " ");
        char *segundo_token = strtok(NULL, " ");
        
    
        // Switch de opções do cliente
        if(strcmp(primeiro_token,"backup") == 0){
                printf("Backup\n");
                
                char path[100]; 
                strcpy(path, "Cliente/"); 
                strcat(path, segundo_token);
                FILE *arq = fopen (path,"rb");
                if(!arq)
                    printf("Erro ao abrir arquivo, verifique se o arquivo existe\n");
                else{   
                    fseek(arq,0L,SEEK_END); // ponteiro para o final do arquivo
                    int tamanhoINT = ftell(arq);
                    printf("TAMANHO:%d\n",tamanhoINT);
                    char tamanho[63];
                    sprintf(tamanho,"%d",tamanhoINT); //converte tamanho INT para um char*
                    fseek(arq, 0L, SEEK_SET); // volta com o ponteiro pro inicio do arquivo
                    // manda msg com o nome do arquivo e tamanho
                    protocolo_t mensagem = criaMensagem(strlen(segundo_token),sequencia,4,segundo_token,CRC);
                    if(sendto(soquete,&mensagem,sizeof(mensagem),0,(struct sockaddr*)&endereco, sizeof(endereco)) ==-1)
                        printf("erro ao enviar mensagem\n");
                    else
                    {

                        #ifdef _DEBUG_
                        
                        printf("Mensagem enviada com sucesso, Aguardando resposta:\n");

                        #endif
                        //TODO implementar timout (não lembro se precisava do lado do cliente ou não)
                        while (!recebeResposta(soquete,bufferResposta)){}
                        
                        #ifdef _DEBUG_
                        printf("Resposta recebida:\n");
                        printMensagem(bufferResposta);
                        #endif

                        //TODO tratar resposta
                        switch (getTipo(bufferResposta))
                        {
                        //Recebeu um OK, manda dados
                        case 2:
                            #ifdef _DEBUG_
                            printf("Recebeu um ok\n");

                            #endif

                            // ENVIA TAMANHO
                            sequencia = sequencia + 1;
                            mensagem = criaMensagem(strlen(tamanho),sequencia,0b01111,tamanho,CRC);
                            
                            if(sendto(soquete,&mensagem,sizeof(mensagem),0,(struct sockaddr*)&endereco, sizeof(endereco)) ==-1)
                            {
                                printf("erro ao enviar mensagem\n");
                                continue;
                            }
                            printf("tamanho enviado, aguardando ok\n");
                            while (!recebeResposta(soquete,bufferResposta)){}

                            //TODO tradar outras respostas
                            if(getTipo(bufferResposta) != 2){
                                printf("não recebeu um ok\n");
                                continue;
                            }
                            
                            sequencia = sequencia + 1;
                            mensagem = criaMensagem(0,sequencia,16,"",CRC);
                            char buffer[63]; //Buffer de leitura de arquivo
                            size_t bytesLidos;
                            //TODO implementar sequencia neste loop
                            while ((bytesLidos = fread(buffer, 1, sizeof(buffer), arq)) > 0)
                            {
                                memcpy(mensagem.dados, buffer, bytesLidos);
                                mensagem.tamanho = bytesLidos;

                                sequencia = sequencia + 1;
                                mensagem.sequencia = sequencia;
                                #ifdef _DEBUG_
                                printf("\nMandando pacote:\n");
                                printMensagemEstruturada(mensagem);
                                #endif
                                
                                sendto(soquete,&mensagem,sizeof(mensagem),0,(struct sockaddr*)&endereco, sizeof(endereco));
                                //Aguarda resposta
                                while (!recebeResposta(soquete,bufferResposta)){}
                                #ifdef _DEBUG_
                                printf("\nPacote recebido:\n");
                                printMensagem(bufferResposta);
                                #endif

                                //NACK
                                while(getTipo(bufferResposta) == 1){
                                    #ifdef _DEBUG_
                                    printf("Recebeum um NACK, enviando mensagem novamente\n");
                                    #endif
                                    sendto(soquete,&mensagem,sizeof(mensagem),0,(struct sockaddr*)&endereco, sizeof(endereco));
                                    while (!recebeResposta(soquete,bufferResposta)){}
                                    
                                }
                                //tratar outros erros aqui
                                if(getTipo(bufferResposta) != 0){

                                }
                                //ACK


                            }
                            //Fim da transmissão de dados
                            mensagem = criaMensagem(0,0,17,"Fim da transmissão de dados",0);
                            sendto(soquete,&mensagem,sizeof(mensagem),0,(struct sockaddr*)&endereco, sizeof(endereco));
                            printf("\nBackup feito com sucesso\n\n");
                        break;
                        
                        default:
                            break;
                        }
                    
                    }
                }

        }
        else if(strcmp(primeiro_token, "restaura") == 0){
                printf("Restaura\n");

            }
        else if (strcmp(primeiro_token,"verifica") == 0){
                printf("Verifica\n");
            }
        else{
            printf("opção invalida, tente novamente:\n");
            }
            menu();
            fgets(entrada, 63, stdin);
            entrada[strcspn(entrada, "\n")] = '\0';
    }

    free(entrada);
    free(bufferResposta);
	close(soquete);
    return 0;

}
