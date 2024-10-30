
// criasocket.c
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <linux/if_packet.h>
#include <sys/socket.h>
#include <net/if.h>
#include <arpa/inet.h>
#include <net/ethernet.h>
#include <sys/un.h>   // Para sockets de domínio UNIX

#define MEU_PROTOCOLO 0x88b5
#define SOCKET_PATH "/tmp/socket_envia"

int cria_socket_raw(char *interface) {
    int soquete = socket(AF_PACKET, SOCK_RAW, htons(MEU_PROTOCOLO));
    if (soquete == -1) {
        perror("Erro ao criar socket raw");
        exit(EXIT_FAILURE);
    }
    
    int ifindex = if_nametoindex(interface);
    if (ifindex == 0) {
        perror("Erro ao obter índice da interface");
        close(soquete);
        exit(EXIT_FAILURE);
    }

    struct sockaddr_ll endereco = {0};
    endereco.sll_family = AF_PACKET;
    endereco.sll_protocol = htons(MEU_PROTOCOLO);
    endereco.sll_ifindex = ifindex;

    if (bind(soquete, (struct sockaddr*) &endereco, sizeof(endereco)) == -1) {
        perror("Erro ao fazer bind no socket raw");
        close(soquete);
        exit(EXIT_FAILURE);
    }

    return soquete;
}

int cria_socket_unix() {
    int unix_socket = socket(AF_UNIX, SOCK_STREAM, 0);
    if (unix_socket == -1) {
        perror("Erro ao criar socket UNIX");
        exit(EXIT_FAILURE);
    }

    struct sockaddr_un endereco = {0};
    endereco.sun_family = AF_UNIX;
    strcpy(endereco.sun_path, SOCKET_PATH);
    unlink(SOCKET_PATH);  // Remove o arquivo se ele já existir

    if (bind(unix_socket, (struct sockaddr*)&endereco, sizeof(endereco)) == -1) {
        perror("Erro ao fazer bind no socket UNIX");
        close(unix_socket);
        exit(EXIT_FAILURE);
    }

    if (listen(unix_socket, 5) == -1) {
        perror("Erro ao ouvir no socket UNIX");
        close(unix_socket);
        exit(EXIT_FAILURE);
    }

    return unix_socket;
}

void envia_pacote(int soquete_raw, const char *mensagem, size_t tamanho_mensagem) {
    unsigned char buffer[1024];
    memset(buffer, 0, sizeof(buffer));

    // Define o MAC de broadcast
    struct sockaddr_ll endereco = {0};
    endereco.sll_family = AF_PACKET;
    endereco.sll_protocol = htons(MEU_PROTOCOLO);
    endereco.sll_halen = ETH_ALEN;
    memset(endereco.sll_addr, 0xff, ETH_ALEN);

    int tamanho_pacote = 0;
    memcpy(buffer, endereco.sll_addr, ETH_ALEN);
    tamanho_pacote += ETH_ALEN;

    unsigned char mac_origem[ETH_ALEN] = {0x11, 0x22, 0x33, 0x44, 0x55, 0x66};
    memcpy(buffer + tamanho_pacote, mac_origem, ETH_ALEN);
    tamanho_pacote += ETH_ALEN;

    buffer[tamanho_pacote++] = (MEU_PROTOCOLO >> 8) & 0xFF;
    buffer[tamanho_pacote++] = MEU_PROTOCOLO & 0xFF;

    memcpy(buffer + tamanho_pacote, mensagem, tamanho_mensagem);
    tamanho_pacote += tamanho_mensagem;

    if (sendto(soquete_raw, buffer, tamanho_pacote, 0, (struct sockaddr*)&endereco, sizeof(endereco)) == -1) {
        perror("Erro ao enviar pacote");
    } else {
        printf("Pacote enviado com sucesso!\n");
    }
}

int main() {
    int soquete_raw = cria_socket_raw("wlan0");
    int soquete_unix = cria_socket_unix();

    printf("Aguardando mensagens no socket UNIX...\n");

    struct sockaddr_un cliente_endereco;
    socklen_t cliente_len = sizeof(cliente_endereco);

    int cliente_socket = accept(soquete_unix, (struct sockaddr*)&cliente_endereco, &cliente_len);
    if (cliente_socket == -1) {
        perror("Erro ao aceitar conexão");
        close(soquete_raw);
        close(soquete_unix);
        exit(EXIT_FAILURE);
    }

    char mensagem[512];
    int tamanho_mensagem = recv(cliente_socket, mensagem, sizeof(mensagem), 0);
    if (tamanho_mensagem > 0) {
        envia_pacote(soquete_raw, mensagem, tamanho_mensagem);
    }

    close(cliente_socket);
    close(soquete_raw);
    close(soquete_unix);
    unlink(SOCKET_PATH);

    return 0;
}
