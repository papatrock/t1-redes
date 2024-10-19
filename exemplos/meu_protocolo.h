// meu_protocolo.h
#ifndef MEU_PROTOCOLO_H
#define MEU_PROTOCOLO_H

#include <stdint.h>

struct meu_protocolo_hdr {
    uint8_t tipo;          // Tipo de mensagem
    uint16_t tamanho;      // Tamanho do payload
    uint32_t id;           // Identificador da mensagem
    // Adicione mais campos conforme necessário
} __attribute__((packed));

#endif // MEU_PROTOCOLO_H
