#include "../include/servidor.h"


int main()
{
    int soquete = criaSocket(INTERFACE);

    while (1)
        {
        // buffer precisa ter no minimo 68 bytes
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
            continue;
        }
        else{
            if (bytes_recebidos > 0 && buffer[0] != 0b01111110)
                continue;

            printf("Pacote recebido (%d bytes):\n", bytes_recebidos);
            for (ssize_t i = 0; i < bytes_recebidos; i++) {
                printf("%02x ", (unsigned char)buffer[i]);
                if ((i + 1) % 16 == 0) printf("\n");
            }
            printf("\n");
        }

    }
    close(soquete);
    return 0;
}
