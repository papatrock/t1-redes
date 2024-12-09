#include "../include/servidor.h"

void handle_verifica(unsigned char* mensagem, int soquete, struct sockaddr_ll path_addr,unsigned char sequencia){
    char *dados = getDados(mensagem);
    printf("DADDDOOOOS:%s\n",dados);
    char *path = calloc(7 + sizeof(dados),sizeof(char));
    strcpy(path,"Backup/");
    strcat(path,dados);
    printf("PATHHHHH:%s\n",path);
    protocolo_t resposta; 

    char *checksum_result = checksum(path);
    printf("checksum servidor: %s\n",checksum_result);


    if(!checksum_result){
        //TODO implementar código de erro
        char *erro = "Erro ao fazer checksum, verifique se o arquivo existe\n";
        printf("%s",erro);
        resposta = criaMensagem(strlen(erro),sequencia,ERRO,erro);
        if(!enviaResposta(soquete,path_addr,resposta))
                printf("Erro ao enviar resposta\n");
        free(path);
        return;
    }

    resposta = criaMensagem(strlen(checksum_result),sequencia,OK_CHECKSUM,checksum_result);
    printMensagemEstruturada(resposta);
    if(!enviaResposta(soquete,path_addr,resposta))
                printf("Erro ao enviar resposta\n");
    else
        printf("checksum enviado\n");
    free(path);
}