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

    //TODO implementar timout
    while (!recebeResposta(soquete, bufferResposta, mensagem, endereco)){}

    #ifdef _DEBUG
    printf("Resposta recebida:\n");
    printMensagem(bufferResposta);
    #endif

    unsigned char* data = getDados(bufferResposta);
    long int file_size = -1;
    switch (getTipo(bufferResposta))
    {
    case OK_TAM:
        file_size = atol((char*) data);
        printf("Tamanho do arquivo: %ld\n", file_size);
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
    if(access(path, F_OK) == 0) {
        char restaura_action;
        do
        {
            printf("Já existe um arquivo com esse nome. O que deseja fazer? (Digite o número que está ao lado da opção desejada)\n");
            printf("1. Sobrescrever\n");
            printf("2. Criar um novo arquivo (enumerado, por exemplo <nome_arquivo_x> onde x eh a versao ainda nao existente)\n");
            printf("3. Cancelar\n");

            scanf("%c", &restaura_action);
            switch (restaura_action)
            {
            case '1':
                arq = fopen(path, "w");
                break;
            case '2':
                char *path_new_file = NULL; /* inicializa como NULL para evitar warning */
                int result = get_next_file_version(path, &path_new_file);

                if(result == MEMORY_ALLOCATION_FAILURE) {
                    printf("Falha ao criar arquivo\n");
                    // Handle memory allocation failure
                    return;
                } else if(result == MAX_VERSIONS_REACHED) {
                    printf("Maximo de versoes de arquivos foi alcancada\n");
                    // Handle max versions reached
                    return;
                }
                arq = fopen(path_new_file, "w");
                printf("Arquivo criado com sucesso: %s\n", path_new_file);
                free(path_new_file);
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

    if(!arq) {
        printf("Erro ao abrir arquivo\n");
        // Handle file open error
        return;
    }

    // envia OK para o servidor
    (*sequencia)++;
    mensagem = criaMensagem(0, (*sequencia), OK, "");
    if(sendto(soquete, &mensagem, sizeof(mensagem), 0 ,(struct sockaddr*)&endereco, sizeof(endereco)) == -1) {
        printf("erro ao enviar mensagem\n");
        return;
    }

    // começa a receber dados do arquivo
    while (getTipo(bufferResposta) != FIM_TRANSMISSAO_DADOS){
        while(!recebeResposta(soquete, bufferResposta, mensagem, endereco)){}
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
            (*sequencia)++;
            mensagem = criaMensagem(0,(*sequencia),ACK,"");
            sendto(soquete, &mensagem, sizeof(mensagem), 0 ,(struct sockaddr*)&endereco, sizeof(endereco));
        }
    }
    printf("Terminou de receber dados\n\n");
    fclose(arq);
}