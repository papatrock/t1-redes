#include "../include/cliente.h"


void handle_verifica(char* nome_arq, struct sockaddr_ll endereco, int soquete, unsigned char *sequencia, unsigned char *bufferResposta){
    protocolo_t mensagem = criaMensagem(strlen(nome_arq),(*sequencia),VERIFICA,nome_arq);
    if(sendto(soquete, &mensagem, sizeof(mensagem), 0,(struct sockaddr*)&endereco, sizeof(endereco)) == -1) {
        printf("erro ao enviar mensagem\n");
        return;
    }

    #ifdef _DEBUG_
    printf("Mensagem enviada com sucesso, Aguardando resposta:\n");
    #endif

    while (!recebeResposta(soquete, bufferResposta, mensagem, endereco,sequencia)){}

    #ifdef _DEBUG
    printf("Resposta recebida:\n");
    printMensagem(bufferResposta);
    #endif

    switch (getTipo(bufferResposta))
    {
    case OK_CHECKSUM:
        char *checksum_servidor = getDados(bufferResposta);
        char *path = calloc(strlen("Cliente/") + strlen(nome_arq) + 1, sizeof(char));

        strcpy(path, "Cliente/");   
        strcat(path, nome_arq);
        char *checksum_cliente = checksum(path);
        if(!checksum_cliente){
            printf("Erro ao verifica arquivo, verifique se o arquivo existe\n");
            return;
        }
        #ifdef _DEBUG_
        printf("checksum cliente: %s\nchecksum servidor: %s\n",checksum_cliente,checksum_servidor);

        #endif


        if(strcmp((char*)checksum_cliente,(char*)checksum_servidor) == 0)
            printf("Arquivo atualizado\n");
        else
            printf("Arquivo desatualizado\n");

        free(path);
    break;

    case ERRO:
        printf("Erro ao acessar arquivo no servidor, verifique se ele existe\n");
        break;
    
    default:
        break;
    }
 
}