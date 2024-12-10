#include <stdio.h>
#include <unistd.h> // Para usleep (no Windows, use <windows.h> e Sleep)
#include <pthread.h> // Para trabalhar com threads

// Variável global para controlar o loop de animação
volatile int carregando = 1;

// Função que exibe a animação de carregamento
void *mensagem_carregando(void *mensagem) {
    const char *animacao[] = {".  ", ".. ", "...", "   "}; // Estados da animação
    int frames = sizeof(animacao) / sizeof(animacao[0]);
    int frame = 0;

    // Loop até a variável 'carregando' ser alterada
    while (carregando) {
        printf("\r%s%s", (char *)mensagem, animacao[frame]); // Atualiza os pontos
        fflush(stdout);
        frame = (frame + 1) % frames; // Avança para o próximo estado da animação
        usleep(500000); // Pausa por 500ms
    }

    // Limpa a linha e exibe a mensagem final
    printf("\r%s... concluído!\n", (char *)mensagem);
    return NULL;
}

// Função simulando trabalho (que você substituirá pelo seu código real)
void trabalho_pesado() {
    // Simulação de trabalho pesado com sleep de 5 segundos
    sleep(5);
}

int main() {
    pthread_t thread;

    // Inicia a animação de carregamento em uma thread separada
    pthread_create(&thread, NULL, mensagem_carregando, "Fazendo backup");

    // Executa a função de trabalho pesado
    trabalho_pesado();

    // Termina a animação de carregamento
    carregando = 0;

    // Aguarda a thread de animação terminar
    pthread_join(thread, NULL);

    return 0;
}
