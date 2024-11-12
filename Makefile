CFLAGS = -Wall
clienteObjs = cliente.o soquete-lib.o
servidorObjs = servidor.o soquete-lib.o

all: servidor cliente

debug: CFLAGS += -D_DEBUG_ -g
debug: all

servidor: $(servidorObjs)
	gcc $(servidorObjs) $(CFLAGS) -o servidor

servidor.o: src/servidor.c include/servidor.h
	gcc $(CFLAGS) -c src/servidor.c -o servidor.o

soquete-lib.o: src/soquete-lib.c include/soquete-lib.h
	gcc $(CFLAGS) -c src/soquete-lib.c -o soquete-lib.o

cliente: $(clienteObjs)
	gcc $(clienteObjs) $(CFLAGS) -o cliente

cliente.o: src/cliente.c include/cliente.h
	gcc $(CFLAGS) -c src/cliente.c -o cliente.o

clean:
	rm -f *~ *.o

purge: clean
	rm -f servidor
	rm -f cliente
