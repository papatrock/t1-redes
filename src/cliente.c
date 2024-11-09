#include "../include/cliente.h"


int recebeResposta(int soquete,unsigned char *buffer) {

    struct sockaddr_ll addr;
    socklen_t addr_len = sizeof(addr);
         
    int bytes_recebidos = recvfrom(soquete, buffer, 68, 0, (struct sockaddr*)&addr, &addr_len);
     
    if (bytes_recebidos == -1) {
        fprintf(stderr, "Erro ao receber dados\n");
        free(buffer);
        return 0;
    }
    if(buffer[0] != 0b01111110)
        return 0;

    printf("Resposta recebida (%d bytes):\n", bytes_recebidos);
    return 1;   

}

int main(int argc, char *argv[]){

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
        
    fgets(entrada, 63, stdin);

    // Remove o caractere de nova linha ('\n') que fgets adiciona
    entrada[strcspn(entrada, "\n")] = '\0';

    while (strcmp(entrada, "sair") != 0) {
        // Separar a entrada em dois tokens usando espaço como delimitador
        char *primeiro_token = strtok(entrada, " ");
        char *segundo_token = strtok(NULL, " ");
        
    
        // Switch de opções do cliente
        if(strcmp(primeiro_token,"backup") == 0){
                printf("Backup\n");
                
                //TODO setar tamanho correto
                protocolo_t mensagem = criaMensagem(0,0,4,segundo_token,0);
                if(sendto(soquete,&mensagem,sizeof(mensagem),0,(struct sockaddr*)&endereco, sizeof(endereco)) ==-1)
                {
                    printf("erro ao enviar mensagem\n");
                }
                else
                {

                    printf("Mensagem enviada com sucesso, Aguardando resposta:\n");
                    //TODO implementar timout (não lembro se precisava do lado do cliente ou não)
                    while (!recebeResposta(soquete,bufferResposta)){}
                    
                    printf("Resposta recebida:\n");
                    printMensagem(bufferResposta);
                }

        }
        else if(strcmp(primeiro_token, "restaura") == 0){
                printf("Restaura\n");

            }
        else if (strcmp(primeiro_token,"verifica") == 0){
                printf("Verifica\n");
            }
        else{
            printf("opção invalida, tente novamente:\n");
            }

            fgets(entrada, 63, stdin);
            entrada[strcspn(entrada, "\n")] = '\0';
    }

    free(entrada);
    free(bufferResposta);
	close(soquete);
    return 0;

}
