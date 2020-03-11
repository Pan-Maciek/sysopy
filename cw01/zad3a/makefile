cc = gcc -Wall -std=c11 -g

all: main_static

dirs:
	mkdir -p build

static_lib: dirs lib.c
	$(cc) -c lib.c -o build/lib.o
	ar rcs build/lib.a build/lib.o

shared_lib: dirs lib.c
	$(cc) -c -fPIC lib.c -o build/lib.o
	$(cc) -shared -fPIC -o libdiff.so build/lib.o

main_static: main.c static_lib
	$(cc) -c main.c -o build/main.o
	$(cc) build/main.o build/lib.a -o main.out

main_shared: main.c shared_lib
	$(cc) main.c -L. -o main.out -ldiff -Wl,-rpath=.

main_dynamic: main.c shared_lib
	$(cc) main.c -o main.out -ldl -D dynamic

clean:
	rm -Rf build *.out *.so *.o *.a