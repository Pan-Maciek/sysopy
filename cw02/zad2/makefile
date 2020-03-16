cc = gcc -Wall -g

all: main

main:
	$(cc) -o find main.c

main_nftw:
	$(cc) -o find main.c -D _XOPEN_SOURCE=500 -D use_nftw 

clean:
	rm -Rf find *.txt *.out
