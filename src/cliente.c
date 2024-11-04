#include "../include/cliente.h"

/* TODO 
 *fazer diferentes tipos de mensagem dependendo do tipo
 *
 */
protocolo_t criaMensagem(char *dados, unsigned int tipo) {
    protocolo_t mensagem; 


    mensagem.marcador = 0b01111110;
    mensagem.tamanho = 0b000000;
    mensagem.sequencia = 0b00000;
    mensagem.tipo = tipo;
    strncpy((char *)mensagem.dados, dados, sizeof(mensagem.dados) - 1);
    mensagem.CRC = 0b00000000;

    return mensagem;
}

int recebeResposta(int soquete) {
    unsigned char *buffer = (unsigned char *)malloc(68 * sizeof(unsigned char));
    if (!buffer) {
        printf("erro ao alocar buffer\n");
        return -1;
    }

    struct sockaddr_ll addr;
    socklen_t addr_len = sizeof(addr);
         
    int bytes_recebidos = recvfrom(soquete, buffer, 68, 0, (struct sockaddr*)&addr, &addr_len);
    
    if (bytes_recebidos == -1) {
        fprintf(stderr, "Erro ao receber dados\n");
        free(buffer);
        return 0;
    } else {
        printf("Pacote recebido (%d bytes):\n", bytes_recebidos);
        for (ssize_t i = 0; i < bytes_recebidos; i++) {
            printf("%02x ", (unsigned char)buffer[i]);
            if ((i + 1) % 16 == 0) printf("\n");
        }
        printf("\n");
        
        if (bytes_recebidos >= 3 && memcmp(buffer, "ACK", 3) == 0) {
            free(buffer);
            return 1; // Resposta válida recebida
        }
    }

    free(buffer);
    return 0;
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
        

        fgets(entrada, 63, stdin);
        entrada[strcspn(entrada, "\n")] = '\0';
    }

	protocolo_t mensagem = criaMensagem("qualquer coisa pra ve se muda os dados ali",0);
	printf("Mensagem->dados:%s\n",mensagem.dados);
    if(sendto(soquete,&mensagem,sizeof(mensagem),0,(struct sockaddr*)&endereco, sizeof(endereco)) ==-1)
	{
		printf("erro ao enviar mensagem\n");
	}
	else{

		printf("Mensagem enviada com sucesso, aguardando resposta\n");
        int resposta = recebeResposta(soquete);
        printf("resposta = %d\n",resposta);

    }

	close(soquete);
    return 0;

}
