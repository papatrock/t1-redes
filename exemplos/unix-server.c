    #include <stdio.h>
    #include <errno.h>
    #include <sys/socket.h>
    #include <sys/un.h>
     
    int main (int argc, char *argv[])
    {
       int serversock;
       struct sockaddr_un self;
     
       /* cria o socket */
       serversock = socket (AF_LOCAL, SOCK_STREAM, 0) ;
       if ( serversock < 0 )
       {
          perror("Erro no socket");
          exit(1);
       }
     
       /* monta o endereço */
       bzero(&self, sizeof(self));
       self.sun_family = AF_LOCAL;
       strcpy (self.sun_path, "./socket");
     
       /* associa o endereço ao socket */
       if ( bind (serversock, (struct sockaddr*)&self, sizeof(self)) )
       {
          perror("Erro no bind");
          exit(1);
       }
     
       /* coloca o socket em escuta */
       if ( listen (serversock, 20) )
       {
          perror("Erro no listen");
          exit(1);
       }
     
       while (1)
       {
          int clientsock;   
          struct sockaddr_un client_addr;
          int addrlen = sizeof(client_addr);
          char * resposta ;
     
          /* aguarda e aceita uma conexão externa */
          clientsock = accept (serversock, (struct sockaddr*)&client_addr, &addrlen);
     
          printf("Client connected\n") ;
     
          /* envia uma resposta HTTP padrão */
          resposta = "Welcome, client !\n" ;
     
          write (clientsock, resposta, strlen (resposta)+1);
     
          /* fecha o socket da conexão recebida */
          close (clientsock);
       }
     
       /* encerra o socket servidor */
       close (serversock);
       unlink ("./socket") ; // o servidor deve remover o arquivo
     
       return 0;
    }

