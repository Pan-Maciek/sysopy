cc = gcc -Wall -ggdb -lpthread

all: main

main:
	$(cc) main.c -o main.out

clean:
	rm -f *.out *.pgm *.txt

get-data:
	wget 'https://people.sc.fsu.edu/~jburkardt/data/pgma/apollonian_gasket.ascii.pgm'
	wget 'https://people.sc.fsu.edu/~jburkardt/data/pgma/brain_398.ascii.pgm'