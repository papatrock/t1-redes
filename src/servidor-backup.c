#include "../include/servidor.h"

void handle_backup(unsigned char* buffer, int soquete, struct sockaddr_ll path_addr,unsigned char sequencia,protocolo_t resposta)
{
    // Abre  pasta Backup e abre (ou criar) o  arquivo com o nome solicitado para receber dados
    char path[100]; 
    strcpy(path, "Backup/"); 
    printf("%s\n",(char*)buffer);
    strcat(path, (char*)getDados(buffer)); 
    FILE *arq = fopen (path,"wb+");
    if(!arq)
    {
        printf("erro ao abrir o arquivo, enviando nack\n");
        //TODO troca nack para código de erro
        resposta = criaMensagem(strlen("Erro ao abrir arquivo"),sequencia,NACK,"Erro ao abrir arquivo",0);
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
        resposta = criaMensagem(0,0,OK,"Ok!",0);
        if(!enviaResposta(soquete,path_addr,resposta))
                printf("Erro ao enviar resposta\n");
        else
            printf("Resposta enviada com sucesso, aguardando tamanho\n");
        sequencia = sequencia + 1;

        recebeResposta(soquete,buffer);
        //TODO verificar se cabe em disco
        #ifdef _DEBUG_
        printf("recebeu dados:\n");
        printMensagem(buffer);
        #endif
        // SE COUBER:
        
        resposta = criaMensagem(3,sequencia,OK,"Ok!",0);
        
        if(!enviaResposta(soquete,path_addr,resposta))
            printf("Erro ao enviar resposta\n");
        else
            printf("Resposta enviada com sucesso, aguardando dados\n");
        

        //TODO tratar erros aqui
        //RECEBENDO DADOS  
        while (getTipo(buffer) != FIM_TRANSMISSAO_DADOS){

            while(!recebeResposta(soquete,buffer)){}
            //dados
            if(getTipo(buffer) == DADOS){
                #ifdef _DEBUG_ 
                printf("Recebeu um pacote de dados:\n");
                printMensagem(buffer);
                #endif
                //TODO verificar erro nos dados aqui

                char dados[63];
                memset(dados, 0, sizeof(dados)); // limpa o buffer
                memcpy(dados, getDados(buffer), getTamanho(buffer));
                fwrite(dados,getTamanho(buffer),1,arq);
                resposta = criaMensagem(0,sequencia,ACK,"",0);
                enviaResposta(soquete,path_addr,resposta);
                sequencia = sequencia + 1;
            }
        }
        printf("Terminou de receber dados\n");
        fclose(arq);
    }
}