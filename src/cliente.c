#include "../include/cliente.h"


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
                    //TODO tratar resposta
                    switch (getTipo(bufferResposta))
                    {
                    //Recebeu um OK, manda dados
                    case 2:
                        printf("Recebeu um ok\n");
                        char path[100]; 
                        strcpy(path, "Cliente/"); 
                        strcat(path, segundo_token);
                        FILE *arq = fopen (path,"r");
                        if(!arq)
                        {
                            printf("erro ao abrir o arquivo, enviando fim do envio de dados\n");
                            mensagem = criaMensagem(0,0,17,"Erro ao abrir arquivo\n",0);
                            if(sendto(soquete,&mensagem,sizeof(mensagem),0,(struct sockaddr*)&endereco, sizeof(endereco)) ==-1)
                                printf("Erro ao enviar resposta\n");
                            else
                                printf("Resposta enviada com sucesso\n");
                        }
                        else
                        {
                            printf("Abriu o arquivo\n");
                            mensagem = criaMensagem(0,0,16,"",0);
                            char buffer[63];
                            size_t bytesLidos;
                            //TODO implementar sequencia neste loop
                            while ((bytesLidos = fread(buffer, 1, sizeof(buffer), arq)) > 0)
                            {
                                printf("Mandando pacote:\n");
                                memcpy(mensagem.dados, buffer, bytesLidos);
                                sendto(soquete,&mensagem,sizeof(mensagem),0,(struct sockaddr*)&endereco, sizeof(endereco));

                            }
                            mensagem = criaMensagem(0,0,17,"Fim da transmissão de dados",0);
                        }



                        break;
                    
                    default:
                        break;
                    }
                
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
