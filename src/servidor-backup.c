#include "../include/servidor.h"

void handle_backup(unsigned char* buffer, int soquete, struct sockaddr_ll path_addr,unsigned char *sequencia)
{
    // Abre  pasta Backup e abre (ou criar) o  arquivo com o nome solicitado para receber dados
    char path[100]; 
    protocolo_t resposta;
    strcpy(path, "Backup/"); 
    printf("%s\n",(char*)buffer);
    strcat(path, (char*)getDados(buffer)); 
    FILE *arq = fopen (path,"wb+");
    if(!arq)
    {
        printf("erro ao abrir o arquivo, enviando nack\n");
        //TODO troca nack para código de erro
        resposta = criaMensagem(strlen("Erro ao abrir arquivo"),*sequencia,NACK,"Erro ao abrir arquivo");
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
        resposta = criaMensagem(0,*sequencia,OK,"Ok!");
        if(!enviaResposta(soquete,path_addr,resposta))
                printf("Erro ao enviar resposta\n");

        *sequencia = (*sequencia + 1) % 32;

        recebeResposta(soquete,buffer, resposta, path_addr,sequencia);
        //TODO verificar se cabe em disco
        #ifdef _DEBUG_
        printf("recebeu o tamanho:\n");
        printMensagem(buffer);
        #endif

        // SE COUBER:
        unsigned long espaco_disponivel = verificar_espaco_disco("/");
        if(atoi(getDados(buffer)) > espaco_disponivel){
            //sem espaço no disco, envia um erro
            resposta = criaMensagem(strlen("Sem espaco disponivel"),*sequencia,ERRO,"Sem espaco disponivel");
        
            if(!enviaResposta(soquete,path_addr,resposta))
                printf("Erro ao enviar resposta\n");
            return;
        }
        resposta = criaMensagem(3,*sequencia,OK,"Ok!");
        
        if(!enviaResposta(soquete,path_addr,resposta))
            printf("Erro ao enviar resposta\n");
        
        *sequencia = (*sequencia + 1) % 32;
        //TODO tratar erros aqui
        //RECEBENDO DADOS  
        while (getTipo(buffer) != FIM_TRANSMISSAO_DADOS){

            while(!recebeResposta(soquete,buffer, resposta, path_addr,sequencia)){}
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
                resposta = criaMensagem(0,*sequencia,ACK,"");
                enviaResposta(soquete,path_addr,resposta);
            }
            *sequencia = (*sequencia + 1) % 32;
        }
        printf("Terminou de receber dados\n");
        fclose(arq);
    }
}