    #include <stdio.h>
    #include <unistd.h>
    #include <sys/un.h>
    #include <sys/socket.h>
     
    #define BUFSIZE 256
     
    int main (int argc, char *argv[])
    {
       int sock, numbytes;
       struct sockaddr_un server;
       char *pedido ;
       char resposta[BUFSIZE];
     
        /* cria um socket TCP */
        sock = socket(AF_LOCAL, SOCK_STREAM, 0) ;
        if ( sock < 0 )
        {
            perror("Erro na criação do socket:");
            exit(1);
        }
     
        /* inicializa a estrutura com endereço/porta do servidor */
        bzero (&server, sizeof(server));
        server.sun_family = AF_LOCAL;
        strcpy (server.sun_path, "./socket") ;
     
        /* conecta ao servidor */
        if ( connect(sock, (struct sockaddr*)&server, sizeof(server)) != 0 )
        {
            perror("Erro na conexão:");
            exit(1);
        }
     
        /* envia comando HTTP ao servidor */
        pedido = "GET / HTTP/1.0\n\n" ;
        write (sock, pedido, strlen (pedido)) ;
     
        /* lê resposta enviada pelo servidor */
        numbytes = read (sock, &resposta, BUFSIZE) ;
        write (STDOUT_FILENO, resposta, numbytes);
     
       /* encerra a conexão */
       close(sock);
     
       return 0;
    }

