#include "../include/servidor.h"
#include "../include/soquete-lib.h"


int main()
{
    int soquete = criasocket(INTERFACE);

while (1)
    {
        char buffer[2048];
        struct sockaddr_ll addr;
        socklen_t addr_len = sizeof(addr);
        
        int bytes_recebidos = recvfrom(soquete, buffer, sizeof(buffer), 0, (struct sockaddr*)&addr, &addr_len);
        
        if (bytes_recebidos == -1)
        {
            fprintf(stderr, "Erro ao receber dados\n");
            continue;
        }
        
        //printf("Recebido %d bytes\n", bytes_recebidos);

	//processa pacote
	struct iphdr *ip_header = (struct iphdr *)buffer;
        printf("Recebido pacote de %s\n", inet_ntoa(*(struct in_addr *)&ip_header->saddr));
        printf("Tamanho do pacote: %d bytes\n", bytes_recebidos);
    }
    close(soquete);
    return 0;
}
