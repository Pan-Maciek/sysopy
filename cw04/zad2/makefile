cc = gcc -Wall -ggdb

all: test_signal main

main:
	$(cc) main.c -o main.out

test_signal:
	$(cc) test_signal.c -o test_sig.out

clean:
	rm -Rf *.out
