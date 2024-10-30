
// envia_mensagem.c
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>

#define SOCKET_PATH "/tmp/socket_envia"

int main() {
    int soquete_unix = socket(AF_UNIX, SOCK_STREAM, 0);
    if (soquete_unix == -1) {
        perror("Erro ao criar socket UNIX");
        exit(EXIT_FAILURE);
    }

    struct sockaddr_un endereco = {0};
    endereco.sun_family = AF_UNIX;
    strcpy(endereco.sun_path, SOCKET_PATH);

    if (connect(soquete_unix, (struct sockaddr*)&endereco, sizeof(endereco)) == -1) {
        perror("Erro ao conectar no socket UNIX");
        close(soquete_unix);
        exit(EXIT_FAILURE);
    }

    const char *mensagem = "Olá, este é um pacote enviado do programa separado!";
    if (send(soquete_unix, mensagem, strlen(mensagem), 0) == -1) {
        perror("Erro ao enviar mensagem");
    } else {
        printf("Mensagem enviada com sucesso!\n");
    }

    close(soquete_unix);
    return 0;
}
