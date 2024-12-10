#include "../include/servidor-restaura.h"
#include "../include/utils.h"
#include "../include/servidor.h"

void handle_restaura(unsigned char* buffer, int soquete, struct sockaddr_ll path_addr,char *sequencia) {
    #ifdef _DEBUG_ 
    printf("ENTROU NO RESTAURA\n");
    #endif
    protocolo_t resposta;

    // Abre  pasta Backup e lê o  arquivo com o nome solicitado para receber dados
    char path[100];
    strcpy(path, "Backup/");
    strcat(path, getDados(buffer));

    // valida se o arquivo existe
    if(access(path, F_OK) == -1) {
        // arquivo não existe
        printf("Arquivo não encontrado, enviando erro\n");
        //Manda um erro
        char error_message[2];
        setErrorMessage(ARQUIVO_NAO_ENCONTRADO, error_message);
        resposta = criaMensagem(strlen(error_message), *sequencia, ERRO, error_message);
        if(!enviaResposta(soquete, path_addr, resposta))
            printf("Erro ao enviar resposta\n");
        *sequencia = (*sequencia + 1) % 32;

        return;
    }

    // valida se tem permissão de leitura
    if(access(path, R_OK) == -1) {
        // sem permissão de leitua
        printf("Sem permissão de leitura, enviando erro\n");

        //Manda um erro
        char error_message[2];
        setErrorMessage(ARQUIVO_NAO_ENCONTRADO, error_message);
        resposta = criaMensagem(strlen(error_message), *sequencia, ERRO, error_message);
        if(!enviaResposta(soquete, path_addr, resposta))
            printf("Erro ao enviar resposta\n");
        *sequencia = (*sequencia + 1) % 32;

        return;
    }

    off_t size = file_size(path);

    char msg[15];
    sprintf(msg, "%ld", size);
    resposta = criaMensagem(strlen(msg), *sequencia, OK_TAM, msg);
    if(!enviaResposta(soquete, path_addr, resposta))
        printf("Erro ao enviar resposta\n");
    *sequencia = (*sequencia + 1) % 32;

    while(!recebeResposta(soquete, buffer, resposta, path_addr,sequencia)) {}

    char bufferArquivo[63]; //Buffer de leitura de arquivo
    size_t bytesLidos;
    FILE *arq = fopen(path, "rb");
    if(!arq) {
        printf("Erro ao abrir arquivo\n");
        // Handle file open error
        return;
    }
    //TODO implementar 0 neste loop
    while ((bytesLidos = fread(bufferArquivo, 1, sizeof(bufferArquivo), arq)) > 0)
    {
        resposta = criaMensagem(bytesLidos,*sequencia,DADOS,bufferArquivo);

        *sequencia = (*sequencia + 1) % 32;
        #ifdef _DEBUG_
        printf("\nMandando pacote:\n");
        printMensagemEstruturada(resposta);
        #endif
        
        if(!enviaResposta(soquete, path_addr, resposta))
            printf("Erro ao enviar resposta\n");

        //Aguarda resposta
        while (!recebeResposta(soquete,buffer, resposta, path_addr,sequencia)){}
        #ifdef _DEBUG_
        printf("\nPacote recebido:\n");
        printMensagem(buffer);
        #endif

        //NACK
        while(getTipo(buffer) == NACK){
            #ifdef _DEBUG_
            printf("Recebeum um NACK, enviando resposta novamente\n");
            #endif
            if(!enviaResposta(soquete, path_addr, resposta))
                printf("Erro ao enviar resposta\n");
            else
                printf("Resposta enviada com sucesso\n");
            while (!recebeResposta(soquete,buffer, resposta, path_addr,sequencia)){}
        }

        //tratar outros erros aqui
        if(getTipo(buffer) != ACK){

        }
        //ACK
    }
    //Fim da transmissão de dados
    resposta = criaMensagem(0,*sequencia,FIM_TRANSMISSAO_DADOS,"Fim da transmissão de dados");
    if(!enviaResposta(soquete, path_addr, resposta))
        printf("Erro ao enviar resposta\n");
    *sequencia = (*sequencia + 1) % 32;

    printf("\nRestaura feito com sucesso\n\n");
    fclose(arq);

    return;
}
