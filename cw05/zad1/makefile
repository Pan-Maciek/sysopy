cc = gcc -Wall -ggdb

all: main inc

main: main.c
	$(cc) main.c -o main.out

inc: inc.c
	$(cc) inc.c -o inc.out

test: SHELL:=/bin/bash
test: all
	@./main.out <(\
		echo "ls / | wc";\
		echo "./asdfasjlkfadsl";\
		echo "ls -al | sort | head -3";\
		echo "echo 9 | ./inc.out 10 | ./inc.out 80")

clean:
	rm -Rf *.out
