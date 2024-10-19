# Links

https://www.opensourceforu.com/2015/03/a-guide-to-using-raw-sockets/

https://wiki.inf.ufpr.br/maziero/doku.php?id=pua:comunicacao_em_rede&s

https://wiki.inf.ufpr.br/todt/doku.php?id=raw_socket

https://squidarth.com/networking/systems/rc/2018/05/28/using-raw-sockets.html

https://beej.us/guide/bgnet/

https://wiki.inf.ufpr.br/maziero/doku.php?id=espec:configuracao_de_rede_em_unix&s

https://www.youtube.com/watch?v=AS_nxNS6YKY&ab_channel=hoff._world

Recursos Adicionais

    Manuais e Documentação:
        [man 7 raw](https://man7.org/linux/man-pages/man7/raw.7.html) - Documentação do Linux sobre raw sockets.
        RFCs (Request for Comments) - Documentos que definem padrões e protocolos de rede.

    Ferramentas de Desenvolvimento:
        Wireshark: Para capturar e analisar pacotes de rede.
        tcpdump: Ferramenta de linha de comando para captura de pacotes.

    Livros e Tutoriais:
        "Unix Network Programming" por W. Richard Stevens - Um clássico sobre programação de rede em Unix.
        Beej's Guide to Network Programming - Guia online abrangente sobre programação de sockets.

    Bibliotecas e Exemplos:
        Explore exemplos de código em GitHub pesquisando por "raw socket" e "custom protocol".
        
# sys/socket.h

## sockaddr
Estrutura usada para definir o **socke adddres** usado nas funções **bind()**, **connect()**, **getpeername()**,** getockname**, **recvfrom()** e **sendto()**
```
#include <sys/socket.h>
 
struct sockaddr {
   unsigned short    sa_family;    // address family, AF_xxx
   char              sa_data[14];  // 14 bytes of protocol address
}; 
```


## sockaddr_storage
Essa estrutura deve:
* Ser grande o bastante para acomodar o protocolo



# Descrição do trabalho
* Implementar um servidor para backup de arquivos usando Raw Sockets
* A implementação deve ter os seguintes comandos:
    *    backup <nome.arq>
        *    Faz o backup de um arquivo do cliente para o servidor
    *    restaur <nome.arq>
        *    Restaura o arquivo do servidor para o cliente
    *    verifica <nome.arq>
        *    Verifica se o arquivo do cliente e do servidor são iguais (cksum)
*    Seguir o protocolo definido em sala
*    Não usar Bit Fields
*    Timeout Obrigatório
*    Em duplas
*    Relatório obrigatório (impresso)
*    Entrega na última semana de aula, valor 4,0

Mensagem:

![image](https://hackmd.io/_uploads/BJ2Aju-gJx.png)


Maior mensagem: 63 bytes (nome do arquivo)
Para e espera

    