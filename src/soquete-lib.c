#include "../include/soquete-lib.h"
#include <ctype.h>


int criaSocket(char *interface)
{
    int soquete = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ALL));
    
    if(soquete == -1)
    {
        fprintf(stderr,"Erro ao criar socket, verifique se você é root\n");
        exit(-1);
    }
    
    int ifindex = if_nametoindex(interface);

    struct sockaddr_ll endereco = {0};
    endereco.sll_family = AF_PACKET;
    endereco.sll_protocol = htons(ETH_P_ALL);
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
        fprintf(stderr,"Erro ao fazer setsockopt: Verifique se a interface de rede foi especificada corretamente\ndefina a interface no soquete-lib.h\n");
        exit(-1);
    }
    return soquete;
}

void inicializaSockaddr_ll(struct sockaddr_ll *sockaddr, int ifindex, unsigned char *dest_mac) {
    memset(sockaddr, 0, sizeof(struct sockaddr_ll));
    sockaddr->sll_family = AF_PACKET;
    sockaddr->sll_protocol = htons(ETH_P_IP);
    sockaddr->sll_ifindex = ifindex;
    if(dest_mac != 0){
        sockaddr->sll_halen = ETH_ALEN;
        memcpy(sockaddr->sll_addr, dest_mac, 6); // Endereço MAC de destino
    }
}

/*
    Concatena struct mensagem em uma só string
    para facilitar o xor com o polinomio
*/
void empacota(protocolo_t *mensagem, unsigned char *mensagem_concat) {
    memset(mensagem_concat, 0, 67); 

    mensagem_concat[0] = mensagem->marcador;
    mensagem_concat[1] = mensagem->tamanho;
    mensagem_concat[2] = mensagem->sequencia;
    mensagem_concat[3] = mensagem->tipo;

    memcpy(&mensagem_concat[3], mensagem->dados, mensagem->tamanho);
    mensagem_concat[67] = mensagem->CRC;
}


void imprimir_binario(unsigned char *mensagem, size_t tamanho) {
    for (size_t i = 0; i < tamanho; i++) {
        for (int j = 7; j >= 0; j--) {
            printf("%d", (mensagem[i] >> j) & 1); // Extraí o bit de cada byte
        }
    }
    printf("\n");  // Nova linha no final da impressão
}

/**
 * Cria uma mensagem do tipo protocolo_t e a inicializa com os parametros
 *
 * @param tamanho 
 * @param sequencia
 * @param tipo
 * @param dados
 * @param CRC 
 * @return Mensagem inicializada
 */
protocolo_t criaMensagem(unsigned char tamanho, unsigned char sequencia, unsigned char tipo, char *dados, unsigned char CRC) {
    protocolo_t mensagem;

    mensagem.marcador = MARCADOR;
    mensagem.tamanho = 0b00111111 & tamanho;
    mensagem.sequencia = 0b00011111 & sequencia;
    mensagem.tipo = 0b00011111 & tipo;
    memcpy(mensagem.dados, dados, tamanho);
    mensagem.CRC = 0b00000000;

    size_t tamanho_mensagem = 68 * 8;  
    unsigned char *mensagem_concat = malloc(sizeof(unsigned char) * 68);

    empacota(&mensagem, mensagem_concat);
    unsigned char crc = geraCRC(mensagem_concat, tamanho_mensagem);
    mensagem.CRC = mensagem.CRC +  crc;
    return mensagem;
}


int recebeResposta(int soquete,unsigned char *buffer, protocolo_t ultima_mensagem, struct sockaddr_ll endereco) {

    struct sockaddr_ll addr;
    socklen_t addr_len = sizeof(addr);
         
    int bytes_recebidos = recvfrom(soquete, buffer, 68, 0, (struct sockaddr*)&addr, &addr_len);
     
    if (bytes_recebidos == -1) {
        fprintf(stderr, "Erro ao receber dados\n");
        free(buffer);
        return 0;
    }
    if(buffer[0] != 0b01111110)
        return 0;

    // reenvia a ultima mensagem enviada
    if(getTipo(buffer) == NACK) {
        printf("enviou nack\n");
        while(sendto(soquete, &ultima_mensagem, sizeof(ultima_mensagem), 0,(struct sockaddr*)&endereco, sizeof(endereco)) == -1) {}
        return 0;
    }

    return 1;   
}

void print_byte_as_binary(unsigned char byte, int bits) {
    for (int i = bits - 1; i >= 0; i--) {
        printf("%d", (byte >> i) & 1);
    }
}

void printMensagem(unsigned char *mensagem) {
    printf("Marcador: ");
    print_byte_as_binary(getMarcador(mensagem), 8);
    printf("\n");

    printf("Tamanho: ");
    print_byte_as_binary(getTamanho(mensagem), 6);
    
    printf("\n");

    printf("Sequencia: ");
    print_byte_as_binary(getSequencia(mensagem), 5);

    printf("\n");

    printf("Tipo: ");
    print_byte_as_binary(getTipo(mensagem), 5);

    printf("\n");

    printf("Dados (ASCII): ");
        printf("%s", mensagem+3);
    
    printf("\n");

    printf("CRC: %d\n",getCRC(mensagem));

}

void printMensagemEstruturada(protocolo_t mensagem) {
    printf("Marcador: %d",mensagem.marcador);
    printf("\n");

    printf("Tamanho: %d",mensagem.tamanho);
    
    printf("\n");

    printf("Sequencia: %d",mensagem.sequencia);

    printf("\n");

    printf("Tipo: %d",mensagem.tipo);

    printf("\n");

    printf("Dados (ASCII): %s",(char*)mensagem.dados);
    printf("\n");

    printf("CRC: %d\n",mensagem.CRC);

}

void printMensagemEstruturadaBinario(protocolo_t mensagem) {
    printf("Marcador: ");
    print_byte_as_binary(mensagem.marcador,8);
    printf("\n");

    printf("Tamanho: ");
    print_byte_as_binary(mensagem.tamanho,6);
    printf("\n");

    printf("Sequencia: ");
    print_byte_as_binary(mensagem.sequencia,5);
    printf("\n");

    printf("Tipo: ");
    print_byte_as_binary(mensagem.tipo,5);
    printf("\n");

    printf("Dados : ");
    for (int i = 0; i < mensagem.tamanho; i++) {
        print_byte_as_binary(mensagem.dados[i],8);
    }
    printf("\n");

    printf("CRC: ");
    print_byte_as_binary(mensagem.CRC,8);
    printf("\n");

}
unsigned char getMarcador(unsigned char *mensagem){
    return mensagem[0];
}

unsigned char getTamanho(unsigned char *mensagem){
    return mensagem[1];
}

unsigned char getSequencia(unsigned char *mensagem){
    return mensagem[2];
}

unsigned char getTipo(unsigned char *mensagem)
{
    return mensagem[3];
}


unsigned char *getDados(unsigned char *mensagem){
    return &mensagem[4];
}

unsigned char getCRC(unsigned char *mensagem){
    return mensagem[67];
}

char *getErrors(unsigned char *errors) {
    if(strcmp((char*)errors, SEM_PERMISSAO))
        return "Sem permissão para acessar o arquivo";
    else if(strcmp((char*)errors, SEM_ESPACO))
        return "Sem espaço para armazenar o arquivo";
    else if(strcmp((char*)errors, ARQUIVO_NAO_ENCONTRADO))
        return "Arquivo não encontrado";
    else
        return "Um erro desconhecido ocorreu";
}


unsigned char geraCRC(unsigned char *ptr, int tam) {
    unsigned char ptr_copia[tam];  
    memcpy(ptr_copia, ptr, tam);  

    unsigned char buffer = 0;  
    int deslocamento = 0;
    unsigned char crc = 0;

    while (deslocamento <= (int)strlen((char*)ptr_copia)-9) {
        if (ptr_copia[deslocamento] == '0') {
            deslocamento++;
        } else {
            char_to_binary(ptr_copia + deslocamento, 9, &buffer);

            crc = buffer ^ POLINOMIO_DIVISOR;

            for (int j = deslocamento; j < 9; j++) {
                unsigned int mask = 1 << (8 - (j - deslocamento)); 
                
                ptr_copia[deslocamento + j] = (crc & mask) ? '1' : '0'; 
            }

            deslocamento ++; 
        }
    }

    return crc;
}

int verificaCRC(unsigned char *mensagem){
    
    unsigned char resto = geraCRC(mensagem,68*8);
    
    #ifdef _DEBUG_
    printf("RESTO DO CRC:%d\nSe 0 é porq deu certo",resto);
    #endif
    if(resto == 0)
        return 1;

    return 0;

}