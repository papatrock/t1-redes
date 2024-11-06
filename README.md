# Descrição do trabalho
* Implementar um servidor para backup de arquivos usando Raw Sockets
* A implementação deve ter os seguintes comandos :
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

![image](https://github.com/user-attachments/assets/0a52f2aa-d9fe-4da9-896b-d23f27ff9d23)


Maior mensagem: 63 bytes (nome do arquivo)
Para e espera

# Instruções
| Tipo  | Definição
| ------------- | ------------- |
| 00000 | ACK
| 00001 | NACK  |
| 00010 | Ok  |
| 00100 | Backup  |
| 00101 | Restaura  |
| 00110 | Verifica  |
| 01110 | OK + TAM  |
| 01111 | Tamanho  |
| 01101 | OK + checksum  |
| 10000 | Dados  |
| 10001 | Fim da transmissão de dados  |
| 11111 | Erro  |

# TODO
- [x]  resposta no formato do protocolo
- [ ]  implementar loop de resposta
- [ ]  switch de tratamento de entradas //cliente
- [ ]  switch de tipo no servidor
- [ ]  criar diretório back

# Links

https://www.opensourceforu.com/2015/03/a-guide-to-using-raw-sockets/

https://wiki.inf.ufpr.br/maziero/doku.php?id=pua:comunicacao_em_rede&s

https://wiki.inf.ufpr.br/todt/doku.php?id=raw_socket

https://squidarth.com/networking/systems/rc/2018/05/28/using-raw-sockets.html

https://beej.us/guide/bgnet/

https://wiki.inf.ufpr.br/maziero/doku.php?id=espec:configuracao_de_rede_em_unix&s

https://www.youtube.com/watch?v=AS_nxNS6YKY&ab_channel=hoff._world

    
