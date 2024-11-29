#include "../include/cliente.h"

void handle_backup(char *segundo_token,struct sockaddr_ll endereco,int soquete,unsigned char sequencia, unsigned char CRC,unsigned char *bufferResposta)
{       
    printf("Backup\n");
    
    char path[100]; 
    strcpy(path, "Cliente/"); 
    strcat(path, segundo_token);
    FILE *arq = fopen (path,"rb");
    if(!arq)
        printf("Erro ao abrir arquivo, verifique se o arquivo existe\n");
    else {
        fseek(arq,0L,SEEK_END); // ponteiro para o final do arquivo
        int tamanhoINT = ftell(arq);
        char tamanho[63];
        sprintf(tamanho,"%d",tamanhoINT); //converte tamanho INT para um char*
        fseek(arq, 0L, SEEK_SET); // volta com o ponteiro pro inicio do arquivo
        // manda msg com o nome do arquivo e tamanho
        protocolo_t mensagem = criaMensagem(strlen(segundo_token),sequencia,BACKUP,segundo_token,CRC);
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
            case OK:
                #ifdef _DEBUG_
                printf("Recebeu um ok\n");

                #endif

                // ENVIA TAMANHO
                sequencia = sequencia + 1;
                mensagem = criaMensagem(strlen(tamanho),sequencia,TAMANHO,tamanho,CRC);
                
                if(sendto(soquete,&mensagem,sizeof(mensagem),0,(struct sockaddr*)&endereco, sizeof(endereco)) ==-1)
                {
                    printf("erro ao enviar mensagem\n");
                    return;
                }
                printf("tamanho enviado, aguardando ok\n");
                while (!recebeResposta(soquete,bufferResposta)){}

                //TODO tradar outras respostas
                if(getTipo(bufferResposta) != OK){
                    printf("não recebeu um ok\n");
                    return;
                }
                
                sequencia = sequencia + 1;
                mensagem = criaMensagem(0,sequencia,DADOS,"",CRC);
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
                    while(getTipo(bufferResposta) == NACK){
                        #ifdef _DEBUG_
                        printf("Recebeum um NACK, enviando mensagem novamente\n");
                        #endif
                        sendto(soquete,&mensagem,sizeof(mensagem),0,(struct sockaddr*)&endereco, sizeof(endereco));
                        while (!recebeResposta(soquete,bufferResposta)){}
                    }

                    //tratar outros erros aqui
                    if(getTipo(bufferResposta) != ACK){

                    }
                    //ACK
                }
                //Fim da transmissão de dados
                mensagem = criaMensagem(0,0,FIM_TRANSMISSAO_DADOS,"Fim da transmissão de dados",0);
                sendto(soquete,&mensagem,sizeof(mensagem),0,(struct sockaddr*)&endereco, sizeof(endereco));
                printf("\nBackup feito com sucesso\n\n");
                break;
            default:
                break;
            }
        }
    }
}