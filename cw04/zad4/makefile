cc = gcc -Wall -ggdb

all: sender catcher

sender: sender.c
	$(cc) sender.c -o sender.out

catcher: catcher.c
	$(cc) catcher.c -o catcher.out

test: all
	./catcher.out&
	./sender.out `pidof catcher.out` 1000

clean:
	rm -Rf *.out
