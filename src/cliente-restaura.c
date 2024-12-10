#include "../include/cliente-restaura.h"
#include "../include/soquete-lib.h"

int get_next_file_version(char* path, char **path_new_file) {
    int i = 1;
    char file_version[2];

    (*path_new_file) = calloc(100, sizeof(char));
    if(!(*path_new_file)) {
        return MEMORY_ALLOCATION_FAILURE;
    }
    // monta o nome do arquivo com a versao dele no final
    do {
        strcpy((*path_new_file), path);
        strcat((*path_new_file), "_");

        sprintf(file_version, "%d", i);
        strcat((*path_new_file), file_version);
    } while(access((*path_new_file), F_OK) == 0 && i++ < MAX_FILE_VERSION);

    if(i == MAX_FILE_VERSION) {
        free((*path_new_file));
        return MAX_VERSIONS_REACHED;
    }

    return 0;
}

void handle_restaura(char* nome_arq, struct sockaddr_ll endereco, int soquete, unsigned char *sequencia, unsigned char *bufferResposta) {
    // envia nome do arquivo
    protocolo_t mensagem = criaMensagem(strlen(nome_arq),(*sequencia),RESTAURA,nome_arq);
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

    char* data = getDados(bufferResposta);
    long int file_size = -1;
    switch (getTipo(bufferResposta))
    {
    case OK_TAM:
        file_size = atol((char*) data);
        break;
    case ERRO:
        printf("ERRO: %s\n", getErrors(data));
        return;
    default:
        break;
    }

    // TODO FAZER MALLOC E VER SE TEM ESPAÇO SUFICIENTE

    // possivelmente esse tratamento aqui nao faz sentido
    if(file_size == -1) {
        printf("Erro ao obter tamanho do arquivo\n");
        return;
    }

    // identifica se arquivo ja existe. Se existir, pergunta ao usuario a acao que ele quer fazer
    char path[100]; 
    strcpy(path, "Cliente/"); 
    strcat(path, nome_arq);
    FILE *arq;
    
    arq = fopen(path, "wb+");

    if(!arq) {
        printf("Erro ao abrir arquivo\n");
        // Handle file open error
        return;
    }

    // envia OK para o servidor
    (*sequencia) = (*sequencia + 1) % 32;
    mensagem = criaMensagem(0, (*sequencia), OK, "");
    if(sendto(soquete, &mensagem, sizeof(mensagem), 0 ,(struct sockaddr*)&endereco, sizeof(endereco)) == -1) {
        printf("erro ao enviar mensagem\n");
        return;
    }

    // começa a receber dados do arquivo
    while (getTipo(bufferResposta) != FIM_TRANSMISSAO_DADOS){
        while(!recebeResposta(soquete, bufferResposta, mensagem, endereco,sequencia)){}
        //dados
        if(getTipo(bufferResposta) == DADOS){
            #ifdef _DEBUG_ 
            printf("Recebeu um pacote de dados:\n");
            printMensagem(bufferResposta);
            #endif
            //TODO verificar erro nos dados aqui

            char dados[63];
            memset(dados, 0, sizeof(dados)); // limpa o bufferResposta
            memcpy(dados, getDados(bufferResposta), getTamanho(bufferResposta));
            fwrite(dados,getTamanho(bufferResposta),1,arq);
            (*sequencia) = (*sequencia + 1) % 32;
            mensagem = criaMensagem(0,(*sequencia),ACK,"");
            sendto(soquete, &mensagem, sizeof(mensagem), 0 ,(struct sockaddr*)&endereco, sizeof(endereco));
        }
    }
    printf("\n--------------------\nArquivo restaurado com sucesso\n");
    fclose(arq);
}