#include "../include/cliente.h"

int criasocket(char *interface)
{
    /*
     * int socket (int domain, int type, int protocol)
     *  domain:
     *      AF_PACKET:interface usada para acessar diretamente
     *      paacotes de rede no nível da camada de enlace, os
     *      pacotes são recebidos e enviados no seu formato bruto
     *  type:
     *      SOCK_RAW:Fornece acesso direto aos protocolos e interface
     *      de baixo nível
     *  protocol:
     *      
     */ 
    int soquete = socket(AF_PACKET, SOCK_RAW,htons(ETH_P_ALL));
    
    if(soquete == -1)
    {
        fprintf(stderr,"Erro ao criar socket, verifique se você é root\n");
        exit(-1);
    }
   

    /*
     * #include <net/if.h>
     * unsigned int if_nametoindex(const char *ifname);
     *  retorna o index da interface de rede correspondende ao *ifname
     *  retorna 0 em caso de erro
     */

    int ifindex = if_nametoindex(interface);
   if (ifindex == 0) {
        fprintf(stderr, "Erro: a interface %s não existe\n", interface);
        exit(EXIT_FAILURE);
    }


	struct sockaddr_ll endereco = {0};
	endereco.sll_family = AF_PACKET;
	endereco.sll_protocol = htons(ETH_P_ALL);
	endereco.sll_ifindex = ifindex;
    /*
     *  #include <sys/socket.h>
     *  int bind(int sockfd, const struct sockaddr *addr, socklen_t addrlen)
     *      Associa um endereço referenciado pelo addr ao socket referenciado
     *      pelo sockfd. addrlen especifica o tamanho da estrutura de endereço
     *      apontado pelo addr (em bytes)
     */
    if(bind(soquete,(struct sockaddr*)&endereco, sizeof(endereco)) == -1)
    {
        fprintf(stderr, "Erro ao fazer bind no socket: %s\n", strerror(errno));

        exit(-1);
    }
   
    /*
     * #include 
     * struct packet_mreq{
     *      int mr_ifindex;
     *      unsigned short mr_type;
     *      unsigned short mr_alen;
     *      unsigned char mr_address[8];
     *  };
     */

    struct packet_mreq mr = {0};
    mr.mr_ifindex = ifindex;
    // não joga fora o que identifica como lixo: modo promiscuo
    mr.mr_type = PACKET_MR_PROMISC;


    /**
     *  #include <sys/socket.h>
     *      int setsockopt(int socket, int level, int option_name, const void       *      option_value, socklen_t ,option_len);
     *  Seta varias opções do socket
     *
     */
    if(setsockopt(soquete, SOL_PACKET, PACKET_ADD_MEMBERSHIP, &mr, sizeof(mr)) ==-1)
        fprintf(stderr,"Erro ao faze setsockopt: Verifique se a interface de rede foi especificada corretamente\n");
    return soquete;

}


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
