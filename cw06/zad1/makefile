cc = gcc -Wall -ggdb

all: client server

server: server.c
	$(cc) server.c -o server.out

client: client.c
	$(cc) client.c -o client.out

clean:
	rm -f *.out
