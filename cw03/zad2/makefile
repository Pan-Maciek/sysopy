cc = gcc -Wall -ggdb

all: main util dirs

dirs:
	mkdir -p data

main: main.c
	$(cc) -o main.out main.c

util:
	$(cc) -o util.out util.c

clean:
	rm -Rf *.out *.txt data ipc

test: all
	./util.out -min 10 -max 100 -c 100 -g list.txt
	./main.out list.txt 10 10 flock
	./util.out -t list.txt