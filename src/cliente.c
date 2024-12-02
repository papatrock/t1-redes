#include "../include/cliente.h"
#include "../include/cliente-restaura.h"
#include "../include/utils.h"

void menu(){
    printf("\n---------------------\nopções disponiveis:\nbackup <nome do arquivo>\nrestaura <nome do arquivo>\nverifica <nome do arquivo>\nsair\n-----------------\n");
}

int main(int argc, char *argv[]){
    
    unsigned char sequencia,CRC;
    sequencia = CRC = 0;

    int soquete = criaSocket(INTERFACE); 
    int ifindex = if_nametoindex(INTERFACE);
    struct sockaddr_ll endereco;

    unsigned char *bufferResposta = (unsigned char *)malloc(68 * sizeof(unsigned char));
    if (!bufferResposta) {
        printf("erro ao alocar buffer\n");
        return -1;
    }
    
    inicializaSockaddr_ll(&endereco,ifindex,0);
    
    char *entrada = malloc(63 * sizeof(char));
    if (!entrada) {
        fprintf(stderr, "erro ao alocar memória\n");
        return -1;
    }
    menu();
        
    fgets(entrada, 63, stdin);

    // Remove o caractere de nova linha ('\n') que fgets adiciona
    entrada[strcspn(entrada, "\n")] = '\0';

    while (strcmp(entrada, "sair") != 0) {
        
        // Separar a entrada em dois tokens usando espaço como delimitador
        char entrada_copy[100];
        strcpy(entrada_copy,entrada);

        char *primeiro_token = strtok(entrada_copy, " ");
        char *segundo_token = strtok(NULL, " ");
    
        // Switch de opções do cliente
        if(strcmp(primeiro_token,"backup") == 0){
            handle_backup(segundo_token, endereco, soquete, &sequencia, CRC, bufferResposta);
        }
        else if(strcmp(primeiro_token, "restaura") == 0){
            printf("Restaura\n");
            handle_restaura(segundo_token, endereco, soquete, &sequencia, bufferResposta);
        }
        else if (strcmp(primeiro_token,"verifica") == 0){
            printf("Verifica\n");
        }
        else{
            printf("opção invalida, tente novamente:\n");
        }

        menu();
        fflush(stdin);
        fgets(entrada, 63, stdin);
        entrada[strcspn(entrada, "\n")] = '\0';
    }

    free(entrada);
    free(bufferResposta);
	close(soquete);
    return 0;

}
