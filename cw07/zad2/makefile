cc = gcc -Wall -ggdb -lrt -lpthread

all: manager worker1 worker2 worker3

manager: manager.c
	$(cc) manager.c -o manager.out

worker1: worker1.c
	$(cc) worker1.c -o worker1.out

worker2: worker2.c
	$(cc) worker2.c -o worker2.out

worker3: worker3.c
	$(cc) worker3.c -o worker3.out

clean: 
	rm -f *.out
