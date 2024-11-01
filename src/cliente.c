#include "../include/cliente.h"

/* TODO 
 *fazer diferentes tipos de mensagem dependendo do tipo
 *
 */
protocolo_t *criaMensagem(char *dados,unsigned int tipo){
    protocolo_t *mensagem = (protocolo_t *)malloc(sizeof(protocolo_t)); // Alocando memória para a estrutura

    if (!mensagem) {
        printf("Erro ao alocar memória para a mensagem\n");
        return NULL;
    }

    mensagem->dados = (unsigned char *)malloc(63 * sizeof(unsigned char));
    if(!mensagem->dados){
        printf("Erro ao alocar memória\n");
        return NULL;
    }
    mensagem->marcador = 0b01111110;
    mensagem->tamanho = 0b000000;
    mensagem->sequencia = 0b00000;
    mensagem->tipo = 0b00000;
    memcpy(mensagem->dados, dados, 63);
    mensagem->CRC = 0b00000000;

    return mensagem;
}

int main(){

    int soquete = criaSocket(INTERFACE); 
    
    int ifindex = if_nametoindex(INTERFACE);
    struct sockaddr_ll endereco = {0};
    endereco.sll_family = AF_PACKET;
    endereco.sll_protocol = htons(ETH_P_ALL);
    endereco.sll_ifindex = ifindex;

	protocolo_t *mensagem = criaMensagem("seila mano coloca qualquer coisa pra ve se muda os dados ali",0);
	
    if(sendto(soquete,mensagem,sizeof(mensagem),0,(struct sockaddr*)&endereco, sizeof(endereco)) ==-1)
	{
		printf("erro ao enviar mensagem\n");
	}
	else
		printf("Mensagem enviada com sucesso\n");

	close(soquete);
    return 0;

}
