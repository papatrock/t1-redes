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
    unsigned char* data = getDados((*bufferResposta));
    long int file_size = -1;
    switch (getTipo((*bufferResposta)))
    {
    case OK_TAM:
        file_size = atol((char*) data);
        printf("Tamanho do arquivo: %ld\n", file_size);

        break;
    case ERRO:
        printf("ERRO: %s\n", getErrors(data));
        return;
    case NACK:
        printf("NACK\n"); // ADICIONAR TRATAMENTO
        return;
    default:
        break;
    }

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
    if(access(path, F_OK) == 0) {
        char restaura_action;
        do
        {
            printf("Já existe um arquivo com esse nome. O que deseja fazer? (Digite o número que está ao lado da opção desejada)\n");
            printf("1. Sobrescrever\n");
            printf("2. Criar um novo arquivo (enumerado, por exemplo <nome_arquivo_x> onde x eh a versao ainda nao existente)\n");
            printf("3. Cancelar\n");

            scanf("%c", &restaura_action);
            printf("oi\n");
            printf("restaura_action: %c\n", restaura_action);
            switch (restaura_action)
            {
            case '1':
                arq = fopen(path, "w");
                break;
            case '2':
                int i = 1;
                char path_new_file[100];
                char file_version[2];
                // monta o nome do arquivo com a versao dele no final
                do {
                    strcpy(path_new_file, path);
                    strcat(path_new_file, "_");

                    sprintf(file_version, "%d", i);
                    strcat(path_new_file, file_version);
                    i++;
                } while(access(path_new_file, F_OK) == 0 && i < 100);
                if(i == 100) {
                    printf("Numero maximo de versoes atingido\n");
                    // TODO mandar mensagem de erro (criar novo tipo de erro)
                    return;
                }
                arq = fopen(path_new_file, "w");
                break;
            case '3':
                printf("Operação cancelada\n");
                // TODO mandar mensagem de erro (criar novo tipo de erro)
                return;
            default:
                printf("Opção inválida\n");
                break;
            }
        } while(restaura_action != '1' && restaura_action != '2' && restaura_action != '3');
    }
    else
        arq = fopen(path, "w");


}