cc = gcc -Wall -ggdb

all: main util

main: main.c
	$(cc) -o main.out main.c

util: 
	$(cc) -o util.out util.c

clean:
	rm -Rf *.out *.txt