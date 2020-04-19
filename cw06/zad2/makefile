cc = gcc -Wall -ggdb

all: build_client build_server

build_server: server/*
	$(cc) server/main.c -o server.out -lrt

build_client: client/*
	$(cc) client/main.c -o client.out -lrt

clean:
	rm -f *.out
