
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h> // para close
#include <linux/if_packet.h>
#include <sys/socket.h>
#include <net/if.h>
#include <arpa/inet.h> // para htons

#define MEU_PROTOCOLO 0x88b5

int criasocket(char *interface)
{
    int soquete = socket(AF_PACKET, SOCK_RAW, htons(MEU_PROTOCOLO));
    
    if(soquete == -1)
    {
        fprintf(stderr,"Erro ao criar socket, verifique se você é root\n");
        exit(-1);
    }
    
    int ifindex = if_nametoindex(interface);

    struct sockaddr_ll endereco = {0};
    endereco.sll_family = AF_PACKET;
    endereco.sll_protocol = htons(MEU_PROTOCOLO);
    endereco.sll_ifindex = ifindex;

    if(bind(soquete, (struct sockaddr*) &endereco, sizeof(endereco)) == -1)
    {
        fprintf(stderr,"Erro ao fazer bind no socket\n");
        exit(-1);
    }
    
    struct packet_mreq mr = {0};
    mr.mr_ifindex = ifindex;
    mr.mr_type = PACKET_MR_PROMISC;

    if(setsockopt(soquete, SOL_PACKET, PACKET_ADD_MEMBERSHIP, &mr, sizeof(mr)) == -1) {
        fprintf(stderr,"Erro ao fazer setsockopt: Verifique se a interface de rede foi especificada corretamente\n");
    }
    return soquete;
}

int main()
{
    int soquete = criasocket("wlan0");

    while (1)
    {
        char buffer[2048];
        int bytes_recebidos = recv(soquete, buffer, sizeof(buffer), 0);
        
        if (bytes_recebidos == -1)
        {
            fprintf(stderr, "Erro ao receber dados\n");
            continue;
        }
        
        printf("Recebido %d bytes\n", bytes_recebidos);
    }

    close(soquete);
    return 0;
}
