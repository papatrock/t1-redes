CFLAGS = -Wall
clienteObjs = cliente.o
servidorObjs = servidor.o

all: servidor cliente

servidor: $(servidorObjs)
	gcc $(servidorObjs) $(CFLAGS) -o servidor

servidor.o: src/server.c include/server.h
	gcc $(CFLAGS) -c src/server.c -o servidor.o

cliente: $(clienteObjs)
	gcc $(clienteObjs) $(CFLAGS) -o cliente

cliente.o: src/cliente.c include/cliente.h
	gcc $(CFLAGS) -c src/cliente.c -o cliente.o



clean:
	rm -f *~ *.o

purge: clean
	rm -f servidor
	rm -f cliente
