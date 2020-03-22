cc = gcc -Wall -ggdb

all: main worker

main: main.c
	$(cc) -o main.out main.c

worker: worker.c
	$(cc) -o worker.out worker.c

util: 
	$(cc) -o util.out util.c
