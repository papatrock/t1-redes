#include "../include/cliente.h"
#include "../include/soquete-lib.h"


int main(){

    int soquete = criasocket(INTERFACE); 
    
    int ifindex = if_nametoindex(INTERFACE);
    struct sockaddr_ll endereco = {0};
    endereco.sll_family = AF_PACKET;
    endereco.sll_protocol = htons(ETH_P_ALL);
    endereco.sll_ifindex = ifindex;

	const char *mensagem = "seila mano coloca qualquer coisa pra ve se muda os dados ali";
	if(sendto(soquete,mensagem,strlen(mensagem),0,(struct sockaddr*)&endereco, sizeof(endereco)) ==-1)
	{
		printf("erro ao enviar mensagem\n");
	}
	else
		printf("Mensagem enviada com sucesso\n");

	close(soquete);
    return 0;

}
