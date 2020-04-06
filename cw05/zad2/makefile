cc = gcc -Wall -ggdb

main: main.c
	$(cc) main.c -o main.out

test: main
	@./main.out makefile

clean:
	rm main.out
