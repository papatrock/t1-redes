#include "../include/cliente.h"

void handle_backup(char *segundo_token,struct sockaddr_ll endereco,int soquete,unsigned char *sequencia,unsigned char *bufferResposta)
{       
    
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
        // manda msg com o nome do arquivo
        //primeira msg, sequencia = 0
        protocolo_t mensagem = criaMensagem(strlen(segundo_token),(*sequencia),BACKUP,segundo_token);
        if(sendto(soquete,&mensagem,sizeof(mensagem),0,(struct sockaddr*)&endereco, sizeof(endereco)) ==-1)
            printf("erro ao enviar mensagem\n");
        else
        {

            #ifdef _DEBUG_
            
            printf("Mensagem enviada com sucesso, Aguardando resposta:\n");

            #endif
            //TODO implementar timout (não lembro se precisava do lado do cliente ou não)
            while (!recebeResposta(soquete,bufferResposta, mensagem, endereco,sequencia)){
                
            }
            
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
                (*sequencia) = (*sequencia + 1) % 32;
                mensagem = criaMensagem(strlen(tamanho),(*sequencia),TAMANHO,tamanho);
                
                if(sendto(soquete,&mensagem,sizeof(mensagem),0,(struct sockaddr*)&endereco, sizeof(endereco)) ==-1)
                {
                    printf("erro ao enviar mensagem\n");
                    return;
                }
                while (!recebeResposta(soquete,bufferResposta, mensagem, endereco,sequencia)){}

                //TODO tradar outras respostas
                if(getTipo(bufferResposta) == ERRO){
                    
                    printf("Erro: %s\n",getDados(bufferResposta));
                    return;
                }
                
                (*sequencia) = (*sequencia + 1) % 32;
                char buffer[63]; //Buffer de leitura de arquivo
                size_t bytesLidos;

                //TODO implementar sequencia neste loop
                while ((bytesLidos = fread(buffer, 1, sizeof(buffer), arq)) > 0)
                {
                    mensagem = criaMensagem(bytesLidos,*sequencia,DADOS,buffer);

                    #ifdef _DEBUG_
                    printf("\nMandando pacote:\n");
                    printMensagemEstruturada(mensagem);
                    #endif
                    sendto(soquete,&mensagem,sizeof(mensagem),0,(struct sockaddr*)&endereco, sizeof(endereco));
                    //Aguarda resposta
                    while (!recebeResposta(soquete,bufferResposta, mensagem, endereco,sequencia)){}

                    (*sequencia) = (*sequencia + 1) % 32;
                    #ifdef _DEBUG_
                    printf("\nPacote recebido:\n");
                    printMensagem(bufferResposta);
                    #endif

                    //tratar outros erros aqui
                    if(getTipo(bufferResposta) != ACK){

                    }
                    //ACK
                    
                }
                //Fim da transmissão de dados
                mensagem = criaMensagem(0,*sequencia,FIM_TRANSMISSAO_DADOS,"Fim da transmissão de dados");
                sendto(soquete,&mensagem,sizeof(mensagem),0,(struct sockaddr*)&endereco, sizeof(endereco));
                (*sequencia) = (*sequencia + 1) % 32;
                printf("\nBackup feito com sucesso\n\n");
                break;
            default:
                break;
            }
        }
    }
}