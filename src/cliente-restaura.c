#include "../include/cliente-restaura.h"
#include "../include/soquete-lib.h"

void handle_restaura(char* nome_arq, struct sockaddr_ll endereco, int soquete, unsigned char **bufferResposta) {
    // envia nome do arquivo
    protocolo_t mensagem = criaMensagem(strlen(nome_arq),0,RESTAURA,nome_arq,0);
    if(sendto(soquete, &mensagem, sizeof(mensagem), 0 ,(struct sockaddr*)&endereco, sizeof(endereco)) == -1) {
        printf("erro ao enviar mensagem\n");
        return;
    }

    #ifdef _DEBUG_
    printf("Mensagem enviada com sucesso, Aguardando resposta:\n");
    #endif

    //TODO implementar timout
    while (!recebeResposta(soquete, (*bufferResposta))){}

    #ifdef _DEBUG
    printf("Resposta recebida:\n");
    printMensagem(bufferResposta);
    #endif

    //TODO tratar resposta
    switch (getTipo((*bufferResposta)))
    {
    case OK_TAM:
        unsigned char* tamanho = getDados((*bufferResposta));
        printf("Tamanho do arquivo: %s\n", tamanho);
        break;
    case ERRO:
        char* erro = getErrors((*bufferResposta));
        printf("ERRO: %s\n", erro);
        break;
    
    default:
        break;
    }

    // char path[100]; 
    // strcpy(path, "Cliente/"); 
    // strcat(path, nome_arq);
    // FILE *arq;
    // if(access(path, F_OK) == 0) {
    //     printf("Já existe um arquivo com esse nome. O que deseja fazer? (Digite o número que está ao lado da opção desejada)\n");
    //     printf("1. Sobrescrever\n");
    //     printf("2. Criar um novo arquivo\n");
    //     printf("3. Cancelar\n");

    //     char restaura_action;
    //     scanf("%c", &restaura_action);
    //     switch (restaura_action)
    //     {
    //     case '1':
    //         arq = fopen(path, "w");
    //         break;
    //     case '2':
    //         int i = 1;
    //         char path_new_file[100];
    //         strcpy(path_new_file, path);

    //     default:
    //         break;
    //     }
    // }

}