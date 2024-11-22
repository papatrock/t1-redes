#include "../include/servidor-restaura.h"
#include "../include/utils.h"
#include "../include/servidor.h"

void handle_restaura(unsigned char* buffer, int soquete, struct sockaddr_ll path_addr) {
    #ifdef _DEBUG_ 
    printf("ENTROU NO RESTAURA\n");
    #endif
    protocolo_t resposta;

    // Abre  pasta Backup e lê o  arquivo com o nome solicitado para receber dados
    char path[100];
    strcpy(path, "Backup/");
    strcat(path, (char*)getDados(buffer));

    // valida se o arquivo existe
    if(access(path, F_OK) == -1) {
        // arquivo não existe
        printf("Arquivo não encontrado, enviando erro\n");
        //Manda um erro
        resposta = criaMensagem(strlen(ARQUIVO_NAO_ENCONTRADO), 0, ERRO, ARQUIVO_NAO_ENCONTRADO, 0);
        if(!enviaResposta(soquete, path_addr, resposta))
            printf("Erro ao enviar resposta\n");
        else
            printf("Resposta enviada com sucesso\n");

        return;
    }

    // valida se tem permissão de leitura
    if(access(path, R_OK) == -1) {
        // sem permissão de leitua
        printf("Sem permissão de leitura, enviando erro\n");

        //Manda um erro
        resposta = criaMensagem(strlen(SEM_PERMISSAO), 0, ERRO, SEM_PERMISSAO, 0);
        if(!enviaResposta(soquete, path_addr, resposta))
            printf("Erro ao enviar resposta\n");
        else
            printf("Resposta enviada com sucesso\n");

        return;
    }

    off_t size = file_size(path);

    char msg[15];
    sprintf(msg, "%ld", size);
    resposta = criaMensagem(strlen(msg), 0, OK_TAM, msg, 0);
    if(!enviaResposta(soquete, path_addr, resposta))
        printf("Erro ao enviar resposta\n");
    else
        printf("Resposta enviada com sucesso\n");

    return;
}
