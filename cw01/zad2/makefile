cc = gcc -Wall -std=c11 -g

all: main

dirs:
	mkdir -p build

static_lib: dirs lib.c
	$(cc) -c lib.c -o build/lib.o
	ar rcs build/lib.a build/lib.o

main: main.c static_lib
	$(cc) -c main.c -o build/main.o
	$(cc) build/main.o build/lib.a -o main.out

clean:
	rm -Rf build *.out *.so *.o *.a