cc = gcc -Wall -ggdb -lpthread

all: main

main:
	$(cc) main.c -o main.out

clean:
	rm -f *.out *.pgm *.txt

get-data:
	[ -f 'apollonian_gasket.ascii.pgm' ] || wget 'https://people.sc.fsu.edu/~jburkardt/data/pgma/apollonian_gasket.ascii.pgm'

times = Times.txt
test: get-data main
	@echo > $(times)
	@for thread_count in 1 2	4 8; do\
		for type in sign block interleaved; do\
			echo "threads: $$thread_count $$type" >> $(times);\
			./main.out $$thread_count $$type apollonian_gasket.ascii.pgm /dev/null >> $(times);\
			echo >> $(times);\
		done; done
	@echo Done!
