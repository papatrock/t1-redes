#include "../include/cliente.h"


unsigned char *recebeResposta(int soquete) {
    unsigned char *buffer = (unsigned char *)malloc(68 * sizeof(unsigned char));
    if (!buffer) {
        printf("erro ao alocar buffer\n");
        return NULL;
    }

    struct sockaddr_ll addr;
    socklen_t addr_len = sizeof(addr);
         
    int bytes_recebidos = recvfrom(soquete, buffer, 68, 0, (struct sockaddr*)&addr, &addr_len);
     
    if (bytes_recebidos == -1) {
        fprintf(stderr, "Erro ao receber dados\n");
        free(buffer);
        return NULL;
    }
    printf("Pacote recebido (%d bytes):\n", bytes_recebidos);
    return buffer;   

}

int main(int argc, char *argv[]){


    int soquete = criaSocket(INTERFACE); 
    
    int ifindex = if_nametoindex(INTERFACE);
    struct sockaddr_ll endereco;
    
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
            
            protocolo_t mensagem = criaMensagem(segundo_token,4);
            if(sendto(soquete,&mensagem,sizeof(mensagem),0,(struct sockaddr*)&endereco, sizeof(endereco)) ==-1)
            {
                printf("erro ao enviar mensagem\n");
            }
            else
            {
                printf("Mensagem enviada com sucesso, Mensagem:\n");
                printf("mensagem.tipo:%d\n",mensagem.tipo);
                unsigned char *resposta = recebeResposta(soquete);
                printMensagem(resposta);
            }

            
            /*            char dados[63]; 

            FILE *arq = fopen (segundo_token,"r");
            if(!arq)
            {
                printf("erro ao abrir o arquivo\n");
                return 1;
            }
            fgets(dados,PACOTE,arq);

            //SE OK COMEÇA A MANDAR O ARQUIVO
            while(!feof(arq)){
                             
                protocolo_t mensagem = criaMensagem(dados,0b10000);
                printf("Mensagem->dados:%s\n",mensagem.dados);
                if(sendto(soquete,&mensagem,sizeof(mensagem),0,(struct sockaddr*)&endereco, sizeof(endereco)) ==-1)
                {
                    printf("erro ao enviar mensagem\n");
                }
                else{

                    printf("Mensagem enviada com sucesso, aguardando resposta\n");
                    unsigned char *resposta = recebeResposta(soquete);
                    printMensagem(resposta);
                }

                fgets(dados,PACOTE,arq);
            }

        

        fclose(arq);
*/
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

	/*protocolo_t mensagem = criaMensagem("qualquer coisa pra ve se muda os dados ali",0);
	printf("Mensagem->dados:%s\n",mensagem.dados);
    if(sendto(soquete,&mensagem,sizeof(mensagem),0,(struct sockaddr*)&endereco, sizeof(endereco)) ==-1)
	{
		printf("erro ao enviar mensagem\n");
	}
	else{

		printf("Mensagem enviada com sucesso, aguardando resposta\n");
        unsigned char *resposta = recebeResposta(soquete);
        printMensagem(resposta);
    }
    */

    free(entrada);
	close(soquete);
    return 0;

}
