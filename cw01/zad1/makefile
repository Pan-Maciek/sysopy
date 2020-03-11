cc = gcc -Wall -std=c11 -g

all: static_lib shared_lib

dirs:
	mkdir -p build

static_lib: dirs lib.c
	$(cc) -c lib.c -o build/lib.o
	ar rcs lib.a build/lib.o

shared_lib: dirs lib.c
	$(cc) -c -fPIC lib.c -o build/lib.o
	$(cc) -shared -fPIC -o libdiff.so build/lib.o

clean:
	rm -Rf build *.out *.so *.o *.a
