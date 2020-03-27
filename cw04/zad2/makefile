cc = gcc -Wall -ggdb

all: test_signal main

main: main.c
	$(cc) main.c -o main.out

test_signal: signal_test.c
	$(cc) signal_test.c -o sig_test.out
	$(cc) exec.c -o exec.out

test: all
	./main.out

clean:
	rm -Rf *.out
