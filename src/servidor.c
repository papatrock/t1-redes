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
        else{
           /*struct ethhdr *eth = (struct ethhdr *)(buffer);
            printf("\nEthernet header\n");
            printf("\t|-Source Address : %.2X-%.2X-%.2X-%.2X-%.2X-%.2X\n",eth->h_source[0],eth->h_source[1],eth->h_source[2],eth->h_source[3],eth->h_source[4],eth->h_source[5]);


        printf("\t|-Destination Address : %.2X-%.2X-%.2X-%.2X-%.2X-%.2X\n",eth->h_dest[0],eth->h_dest[1],eth->h_dest[2],eth->h_dest[3],eth->h_dest[4],eth->h_dest[5]);


        printf("\t|-Protocol : %d\n",eth->h_proto);*/
        printf("Pacote recebido (%d bytes):\n", bytes_recebidos);
        for (ssize_t i = 0; i < bytes_recebidos; i++) {
            printf("%02x ", (unsigned char)buffer[i]);
            if ((i + 1) % 16 == 0) printf("\n");
        }
        printf("\n");


        }
        

	//processa pacote


    }
    close(soquete);
    return 0;
}
