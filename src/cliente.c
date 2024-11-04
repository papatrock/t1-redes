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
	else
		printf("Mensagem enviada com sucesso\n");

	close(soquete);
    return 0;

}
