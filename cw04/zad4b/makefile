cc = gcc -Wall -ggdb

all: sender catcher

sender: sender.c
	$(cc) sender.c -o sender.out

catcher: catcher.c
	$(cc) catcher.c -o catcher.out

n = 1000000
test: all
	@echo "expected [$n]"
	@./catcher.out - &
	@./sender.out `pidof catcher.out` $n

clean:
	rm -Rf *.out
