#include "../include/soquete-lib.h"


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
protocolo_t criaMensagem(unsigned char tamanho,unsigned char sequencia,unsigned char tipo,char *dados,unsigned char CRC) {
    protocolo_t mensagem; 
    
    mensagem.marcador = 126;
    mensagem.tamanho = 0b00111111 & tamanho;
    mensagem.sequencia = 0b00011111 & sequencia;
    mensagem.tipo = 0b00011111 & tipo;
    strncpy((char *)mensagem.dados, dados, sizeof(mensagem.dados) - 1);
    mensagem.CRC = geraCRC(mensagem);

    return mensagem;
}


int recebeResposta(int soquete,unsigned char *buffer) {

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
    for (int i = 3; i < 66; i++) {
        printf("%c", mensagem[i]);
    }
    printf("\n");

    printf("CRC:\n");
    print_byte_as_binary(getCRC(mensagem), 8);
    printf("\n");

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

unsigned char *getCRC(unsigned char *mensagem){
    return mensagem[12];
}

char *getErrors(unsigned char *mensagem) {
    char errors = (char) (*getDados(mensagem));

    switch (errors)
    {
    case SEM_PERMISSAO:
        return "Sem permissão para acessar o arquivo";
        break;
    case SEM_ESPACO:
        return "Sem espaço para armazenar o arquivo";
        break;
    case ARQUIVO_NAO_ENCONTRADO:
        return "Arquivo não encontrado";
        break;
    default:
        return "Um erro desconhecido ocorreu";
        break;
    }
}

unsigned char geraCRC(protocolo_t mensagem){
    unsigned char buffer[sizeof(mensagem)];
    size_t pos = 0;

    buffer[pos++] = mensagem.marcador;
    buffer[pos++] = mensagem.tamanho & 0b00111111;
    buffer[pos++] = mensagem.sequencia & 0b00011111; 
    buffer[pos++] = mensagem.tipo & 0b00011111;      
    memcpy(buffer + pos, mensagem.dados, sizeof(mensagem.dados));
    pos += sizeof(mensagem.dados);

    unsigned char crc = 0; // Inicializa o CRC como 0
    for (size_t i = 0; i < pos; i++) {
        crc ^= buffer[i]; // XOR entre o CRC atual e o byte de dados

        for (int j = 0; j < 8; j++) { // Processa os 8 bits de cada byte
            if (crc & 0b10000000) { // Se o bit mais significativo for 1
                crc = (crc << 1) ^ POLINOMIO_DIVISOR; // Desloca e aplica o polinômio
            } else {
                crc <<= 1; // Apenas desloca
            }
        }
    }
    return crc;

}