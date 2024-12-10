#include "../include/servidor.h"

void handle_verifica(unsigned char* mensagem, int soquete, struct sockaddr_ll path_addr,unsigned char *sequencia){
    char *dados = getDados(mensagem);
    char *path = calloc(7 + strlen(dados),sizeof(char));
    strcpy(path,"Backup/");
    strcat(path,dados);
    protocolo_t resposta; 

    char *checksum_result = checksum(path);


    if(!checksum_result){
        //TODO implementar código de erro
        char *erro = "Erro ao fazer checksum, verifique se o arquivo existe\n";
        printf("%s",erro);
        resposta = criaMensagem(strlen(erro),*sequencia,ERRO,erro);
        if(!enviaResposta(soquete,path_addr,resposta))
                printf("Erro ao enviar resposta\n");
        free(path);
        return;
    }

    resposta = criaMensagem(strlen(checksum_result),*sequencia,OK_CHECKSUM,checksum_result);
    if(!enviaResposta(soquete,path_addr,resposta))
                printf("Erro ao enviar resposta\n");
   
    free(path);
}