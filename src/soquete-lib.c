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


/**
 * Cria uma mensagem do tipo protocolo_t e a inicializa com os parametros
 *
 * @param tamanho 
 * @param sequencia
 * @param tipo
 * @param dados
 * @return Mensagem inicializada
 */
protocolo_t criaMensagem(unsigned char tamanho, unsigned char sequencia, unsigned char tipo, char *dados) {
    protocolo_t mensagem;

    mensagem.marcador = MARCADOR;
    mensagem.tamanho = 0b00111111 & tamanho;
    mensagem.sequencia = 0b00011111 & sequencia;
    mensagem.tipo = 0b00011111 & tipo;
    memcpy(mensagem.dados, dados, tamanho);
    mensagem.CRC = 0b00000000;

    unsigned char *mensagem_concat = calloc(68,sizeof(unsigned char) * 68);
    if (!mensagem_concat) {
        printf("Erro ao alocar memória\n");
        exit(-1);
    }

    mensagem_concat = empacotaStruct(&mensagem);
    unsigned char crc = geraCRC(mensagem_concat);
    mensagem.CRC = mensagem.CRC +  crc;
    #ifdef _DEBUG_
    printf("CRC calculado no criamsg: %u\n",mensagem.CRC);
    #endif

    free(mensagem_concat);
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

    if(getTipo(buffer) == NACK || !verificaCRC(buffer)) {
        #ifdef _DEBUG_
        printf("recebeu um nack ou crc invalido\n");
        #endif
        sendto(soquete, &ultima_mensagem, sizeof(ultima_mensagem), 0,(struct sockaddr*)&endereco, sizeof(endereco));
        return 0;
    }

    return 1;   
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
    return (mensagem[1] & 0b00111111);
}

unsigned char getSequencia(unsigned char *mensagem){
    return mensagem[2] & 0b00011111;
}

unsigned char getTipo(unsigned char *mensagem)
{
    return mensagem[3] & 0b00011111;
}


unsigned char *getDados(unsigned char *mensagem){
    return &mensagem[4];
}

unsigned char getCRC(unsigned char *mensagem){
    return mensagem[67];
}

char *getErrors(unsigned char *errors) {
    switch (errors[0])
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

void setErrorMessage(char error_code, char* error_message) {
    error_message[0] = error_code;
    error_message[1] = '\0';
}


unsigned char geraCRC(unsigned char *ptr) {
    #ifdef _DEBUG_
    printf("\n--------------\ncalculo do crc em:\n%s\n",ptr);
    #endif
    int tam = (int)strlen((char*)ptr);
    unsigned char *ptr_copia = malloc(tam);
    if (!ptr_copia) {
        printf("Erro ao alocar memória\n");
        exit(-1);
    }  
    memcpy(ptr_copia, ptr, tam);  

    unsigned char buffer = 0;  
    int deslocamento = 0;
    unsigned char crc = 0;

    while (deslocamento <= tam -9) {
        if (ptr_copia[deslocamento] == '0') {
            deslocamento++;
        } else {

            char_to_binary(ptr_copia + deslocamento, 9, &buffer);

            crc = buffer ^ POLINOMIO_DIVISOR;
            for (int j = 0; j < 9; j++) {
                unsigned int mask = 1 << (8 - j); 
                
                ptr_copia[deslocamento + j] = (crc & mask) ? '1' : '0'; 
            }

            deslocamento ++; 
        }
    }
    char_to_binary(ptr_copia + deslocamento, 8, &crc);
    #ifdef _DEBUG_
    printf("crc = %u\n-----------fim do gera CRC----------\n",crc);
    #endif
    free(ptr_copia);
    return crc;
}

unsigned char *empacotaStruct(protocolo_t *mensagem) {
    #ifdef _DEBUG_
    printf("mensagem a ser empacotada:\n");
    printMensagemEstruturada(*mensagem);
    #endif
    int tamanho_bits = 68 * 8;  
    unsigned char *empacotado = malloc(tamanho_bits + 1);
    if (!empacotado) {
        printf("Erro ao alocar memória\n");
        exit(-1);
    }

    memset(empacotado, 0, tamanho_bits + 1);

    int pos = 0;

    unsigned char campos[] = {mensagem->marcador, mensagem->tamanho, mensagem->sequencia, mensagem->tipo};
    for (int i = 0; i < 4; i++) {
        for (int bit = 7; bit >= 0; bit--) {
            empacotado[pos++] = ((campos[i] >> bit) & 1) ? '1' : '0';
        }
    }

    for (int i = 0; i < mensagem->tamanho; i++) {
        for (int bit = 7; bit >= 0; bit--) {
            empacotado[pos++] = ((mensagem->dados[i] >> bit) & 1) ? '1' : '0';
        }
    }

    for (int bit = 7; bit >= 0; bit--) {
        empacotado[pos++] = ((mensagem->CRC >> bit) & 1) ? '1' : '0';
    }

    empacotado[tamanho_bits] = '\0';
    #ifdef _DEBUG_
    if(mensagem->tipo == DADOS)
        printf("É UM DADO SENDO EMPACOTADO NA STRUCT\n");
    printf("empacota (struct)\n%s\n",empacotado);
    #endif
    return empacotado;
}


unsigned char *empacota(unsigned char *mensagem) {
    int tamanho_bits = 68 * 8;  
    unsigned char *empacotado = malloc(tamanho_bits + 1); 
    if (!empacotado) {
        printf("Erro ao alocar memória\n");
        exit(-1);
    }

    memset(empacotado, 0, tamanho_bits + 1);

    int pos = 0;
    int tamanho = getTamanho(mensagem);
    unsigned char *dados = getDados(mensagem);
    unsigned char campos[] = {getMarcador(mensagem), getTamanho(mensagem), getSequencia(mensagem), getTipo(mensagem)};
    for (int i = 0; i < 4; i++) {
        for (int bit = 7; bit >= 0; bit--) {
            empacotado[pos++] = ((campos[i] >> bit) & 1) ? '1' : '0';
        }
    }

    for (int i = 0; i < tamanho; i++) {
        for (int bit = 7; bit >= 0; bit--) {
            empacotado[pos++] = ((dados[i] >> bit) & 1) ? '1' : '0';
        }
    }

    for (int bit = 7; bit >= 0; bit--) {
        empacotado[pos++] = ((getCRC(mensagem) >> bit) & 1) ? '1' : '0';
    }

    empacotado[tamanho_bits] = '\0';

    #ifdef _DEBUG_
    printf("empacota (mesagem)\n%s\n",empacotado);
    #endif
    return empacotado;
}


/**
 * Faz a divisão com o polinomio divisor e verifica se o resto é 0 ou não
 *
 * @param mensage
 * @return 1 se resto == 0, 0 caso contrario
 */
int verificaCRC(unsigned char *mensagem){

    unsigned char *mensagem_concat = calloc(68,sizeof(unsigned char) * 68);
    if (!mensagem_concat) {
        printf("Erro ao alocar memória\n");
        exit(-1);
    } 

    mensagem_concat = empacota(mensagem);
    unsigned char resto = geraCRC(mensagem_concat);
    
    #ifdef _DEBUG_
    printf("----------VERIFICA CRC----------------\nRESTO DO CRC:%d\nSe 0 é porq deu certo\n-----------FIM DO VERIFICA ----------------",resto);
    #endif
    if(resto == 0)
        return 1;

    free(mensagem_concat);
    return 0;

}

