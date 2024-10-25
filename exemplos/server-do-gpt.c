
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <linux/if_packet.h>
#include <net/ethernet.h>
#include <sys/socket.h>
#include <net/if.h>
#include <sys/ioctl.h>

#define INTERFACE "wlan0"  // Substitua pela interface de rede correta
#define BUFFER_LEN 2048

int main() {
    int sockfd;
    struct sockaddr_ll sa;
    char buffer[BUFFER_LEN];

    sockfd = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ALL));
    if (sockfd < 0) {
        perror("Erro ao criar socket RAW");
        exit(EXIT_FAILURE);
    }

    // Obter o índice da interface
    struct ifreq ifr;
    strncpy(ifr.ifr_name, INTERFACE, IFNAMSIZ - 1);
    if (ioctl(sockfd, SIOCGIFINDEX, &ifr) < 0) {
        perror("Erro ao obter índice da interface");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    // Configurar a estrutura de endereço
    sa.sll_family = AF_PACKET;
    sa.sll_ifindex = ifr.ifr_ifindex;
    sa.sll_protocol = htons(ETH_P_ALL);

    // Bind no socket para a interface de rede
    if (bind(sockfd, (struct sockaddr *)&sa, sizeof(sa)) < 0) {
        perror("Erro ao fazer bind do socket");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    printf("Servidor aguardando pacotes RAW...\n");

    while (1) {
        int bytes_recebidos = recvfrom(sockfd, buffer, BUFFER_LEN, 0, NULL, NULL);
        if (bytes_recebidos < 0) {
            perror("Erro ao receber pacote");
            continue;
        }

        printf("Pacote recebido com %d bytes: %s\n", bytes_recebidos, buffer);
    }

    close(sockfd);
    return 0;
}
