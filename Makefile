CFLAGS = -Wall
clienteObjs = cliente.o soquete-lib.o utils.o cliente-restaura.o cliente-backup.o
servidorObjs = servidor.o soquete-lib.o utils.o servidor-restaura.o servidor-backup.o

all: servidor cliente

debug: CFLAGS += -D_DEBUG_ -g
debug: all

erro: CFLAGS += -D_SIMULA_ERRO_ -D_DEBUG_ -g
erro: all

servidor: $(servidorObjs)
	gcc $(servidorObjs) $(CFLAGS) -o servidor

servidor.o: src/servidor.c include/servidor.h
	gcc $(CFLAGS) -c src/servidor.c -o servidor.o

servidor-restaura.o: src/servidor-restaura.c include/servidor-restaura.h include/soquete-lib.h
	gcc $(CFLAGS) -c src/servidor-restaura.c -o servidor-restaura.o

servidor-backup.o: src/servidor-backup.c include/servidor.h include/soquete-lib.h include/utils.h
	gcc $(CFLAGS) -c src/servidor-backup.c -o servidor-backup.o
soquete-lib.o: src/soquete-lib.c include/soquete-lib.h
	gcc $(CFLAGS) -c src/soquete-lib.c -o soquete-lib.o

utils.o: src/utils.c include/utils.h
	gcc $(CFLAGS) -c src/utils.c -o utils.o

cliente: $(clienteObjs)
	gcc $(clienteObjs) $(CFLAGS) -o cliente

cliente.o: src/cliente.c include/cliente.h
	gcc $(CFLAGS) -c src/cliente.c -o cliente.o

cliente-restaura.o: src/cliente-restaura.c include/cliente-restaura.h include/soquete-lib.h
	gcc $(CFLAGS) -c src/cliente-restaura.c -o cliente-restaura.o

cliente-backup.o: src/cliente-backup.c include/cliente.h include/soquete-lib.h include/utils.h
	gcc $(CFLAGS) -c src/cliente-backup.c -o cliente-backup.o

clean:
	rm -f *~ *.o

purge: clean
	rm -f servidor
	rm -f cliente
