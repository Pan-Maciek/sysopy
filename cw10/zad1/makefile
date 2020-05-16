cc = gcc -Wall -ggdb

all: client server

client:
	$(cc) client.c -o client.out -lpthread

server:
	$(cc) server.c -o server.out -lpthread

clean:
	rm -f *.out