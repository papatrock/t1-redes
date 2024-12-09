#include "../include/cliente.h"
#include "../include/cliente-restaura.h"
#include "../include/utils.h"

void menu(){
    printf("\n---------------------\nopções disponiveis:\nbackup <nome do arquivo>\nrestaura <nome do arquivo>\nverifica <nome do arquivo>\nsair\n-----------------\n");
}

int main(int argc, char *argv[]){
    
    unsigned char sequencia;
    sequencia = 0;

    int soquete = criaSocket(INTERFACE); 
    int ifindex = if_nametoindex(INTERFACE);
    struct sockaddr_ll endereco;

    unsigned char *bufferResposta = (unsigned char *)malloc(68 * sizeof(unsigned char));
    if (!bufferResposta) {
        printf("erro ao alocar buffer\n");
        return -1;
    }
    
    inicializaSockaddr_ll(&endereco,ifindex,0);
    char entrada[64] = {0};
    char comando[64] = {0};
    char argumento[64] = {0};
    menu();
    fgets(entrada, 63, stdin);

    sscanf(entrada, "%63s %63s", comando, argumento);

    while (strcmp(comando, "sair") != 0) {
        
        // Separar a entrada em dois tokens usando espaço como delimitador
        char entrada_copy[100];
        strcpy(entrada_copy,entrada);

        printf("segundo tok: %s\n",argumento);
    
        // Switch de opções do cliente
        if(strcmp(comando,"backup") == 0){
            handle_backup(argumento, endereco, soquete, &sequencia, bufferResposta);
        }
        else if(strcmp(comando, "restaura") == 0){
            printf("Restaura\n");
            handle_restaura(argumento, endereco, soquete, &sequencia, bufferResposta);
        }
        else if (strcmp(comando,"verifica") == 0){
            printf("Verifica\n");
            handle_verifica(argumento, endereco, soquete, &sequencia, bufferResposta);
        }
        else{
            printf("opção invalida, tente novamente:\n");
        }

        menu();
        fflush(stdin);
        fgets(entrada, 63, stdin);
        sscanf(entrada, "%63s %63s", comando, argumento);
        memset(bufferResposta,0,68 * sizeof(unsigned char));
        
    }

    free(bufferResposta);
	close(soquete);
    return 0;

}
