cc = gcc -Wall -ggdb

all: client server

server: server/* common/*
	$(cc) server/main.c -o server.out

client: client/* common/*
	$(cc) client/main.c -o client.out

clean:
	rm -f *.out
