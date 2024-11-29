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

    memcpy(mensagem.dados, dados, tamanho);
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
    for (int i = 3; i < getTamanho(mensagem); i++) {
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

    printf("Dados (ASCII): %s",(char*)mensagem.dados);
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
    return mensagem[12];
}

char *getErrors(unsigned char *errors) {
    if(strcmp(errors, SEM_PERMISSAO))
        return "Sem permissão para acessar o arquivo";
    else if(strcmp(errors, SEM_ESPACO))
        return "Sem espaço para armazenar o arquivo";
    else if(strcmp(errors, ARQUIVO_NAO_ENCONTRADO))
        return "Arquivo não encontrado";
    else
        return "Um erro desconhecido ocorreu";
}
/*
    Concatena struct mensagem em uma só string
    para facilitar o xor com o polinomio
*/
void empacota(struct protocolo *p, unsigned char *mensagem_concat) {
   
    memset(mensagem_concat, 0, p->tamanho + 6); 

    mensagem_concat[0] = p->marcador;
    mensagem_concat[1] = (p->tamanho << 2) | (p->sequencia >> 3);
    mensagem_concat[2] = (p->sequencia << 5) | (p->tipo);
    
    memcpy(&mensagem_concat[3], p->dados, p->tamanho); 
    
    mensagem_concat[p->tamanho + 3] = p->CRC;
}

unsigned char geraCRC(protocolo_t mensagem){
    unsigned short buffer; // 2 bytes
    unsigned char *tmp = malloc(sizeof(unsigned char)*68);
    
    empacota(&mensagem,tmp);

    /*
    for (int i = 0; i < mensagem.tamanho + 6; i++) {
        //print_byte_as_binary(tmp[i],8);
        printf("%02X ", tmp[i]);
    }
    printf("\n");
    printf("TMP:%s\n",tmp);
    */
    



    return 0b00000000; // temp

}