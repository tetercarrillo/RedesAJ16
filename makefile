all: sem_cli sem_svr

sem_cli: sem_cli.o
	gcc -pthread sem_cli.o -o sem_cli

sem_svr: sem_svr.o
	gcc -pthread sem_svr.o -o sem_svr

clean:
	rm -f *.o *.txt *.gch main || true
	rm sem_svr sem_cli || true