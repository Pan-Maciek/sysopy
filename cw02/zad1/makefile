cc = gcc -Wall -ggdb

all: main

main:
	$(cc) -o main.out main.c

clean:
	rm -Rf *.out *.txt data*

test: main

	echo "type\tcount\tsize\treal\tsys\tuser" > wyniki.txt
	for word_count in 100 10000; do\
		for word_size in 1 4 512 1024 4096 8192; do\
			./main.out generate data $$word_count $$word_size >> wyniki.txt;\
			echo -n "lib\t$$word_count\t$$word_size\t" >> wyniki.txt;\
			./main.out copy data data-$$word_count-$$word_size-lib.txt $$word_count $$word_size lib >> wyniki.txt;\
			echo -n "sys\t$$word_count\t$$word_size\t" >> wyniki.txt;\
			./main.out copy data data-$$word_count-$$word_size-sys.txt $$word_count $$word_size sys >> wyniki.txt;\
		done;\
	done
	
	echo >> wyniki.txt
	echo "type\tcount\tsize\treal\tsys\tuser" >> wyniki.txt
	for word_count in 100 10000; do\
		for word_size in 1 4 512 1024 4096 8192; do\
			echo -n "lib\t$$word_count\t$$word_size\t" >> wyniki.txt;\
			./main.out sort data-$$word_count-$$word_size-lib.txt $$word_count $$word_size lib >> wyniki.txt;\
			echo -n "sys\t$$word_count\t$$word_size\t" >> wyniki.txt;\
			./main.out sort data-$$word_count-$$word_size-sys.txt $$word_count $$word_size sys >> wyniki.txt;\
		done;\
	done

	rm -f data*