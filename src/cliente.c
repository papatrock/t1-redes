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

int recebeResposta(int soquete, struct sockaddr_ll endereco)
{
    int resposta = 0;

    while(!resposta)
    {
        unsigned char *buffer = (unsigned char *)malloc(68 * sizeof(unsigned char));
        if(!buffer){
            printf("erro ao alocar buffer\n");
            return -1;
        }

        struct sockaddr_ll addr;
        socklen_t addr_len = sizeof(addr);
         
        int bytes_recebidos = recvfrom(soquete, buffer, 68, 0, (struct sockaddr*)&addr, &addr_len);
        
        if (bytes_recebidos == -1)
        {
            fprintf(stderr, "Erro ao receber dados\n");
            return 0;
        }
        else{
            if (bytes_recebidos > 0 && buffer[0] == 0b01111110)
            {

            printf("Pacote recebido (%d bytes):\n", bytes_recebidos);
            for (ssize_t i = 0; i < bytes_recebidos; i++) {
                printf("%02x ", (unsigned char)buffer[i]);
                if ((i + 1) % 16 == 0) printf("\n");
            }
            printf("\n");
            resposta = 1;
            }
        }
    }
    return resposta;
}

int main(){

    int soquete = criaSocket(INTERFACE); 
    
    int ifindex = if_nametoindex(INTERFACE);
    struct sockaddr_ll endereco = {0};
    endereco.sll_family = AF_PACKET;
    endereco.sll_protocol = htons(ETH_P_ALL);
    endereco.sll_ifindex = ifindex;

	protocolo_t mensagem = criaMensagem("qualquer coisa pra ve se muda os dados ali",0);
	printf("Mensagem->dados:%s\n",mensagem.dados);
    if(sendto(soquete,&mensagem,sizeof(mensagem),0,(struct sockaddr*)&endereco, sizeof(endereco)) ==-1)
	{
		printf("erro ao enviar mensagem\n");
	}
	else{
		printf("Mensagem enviada com sucesso, aguardando resposta\n");
        int resposta = recebeResposta(soquete, endereco);
        printf("resposta = %d\n",resposta);

    }

	close(soquete);
    return 0;

}
