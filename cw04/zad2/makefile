cc = gcc -Wall -ggdb

all: test_signal main

main:
	$(cc) main.c -o main.out

test_signal:
	$(cc) signal_test.c -o sig_test.out

clean:
	rm -Rf *.out
