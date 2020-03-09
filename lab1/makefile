cc = gcc -Wall -std=c11 -g
opt_lvl = s

all: main_static

static_lib: lib.c
	$(cc) -c lib.c -o build/lib.o
	ar rcs build/lib.a build/lib.o

shared_lib:
	$(cc) -c -fPIC lib.c -o build/lib.o
	$(cc) -shared -fPIC -o libdiff.so build/lib.o

main_static: main.c static_lib
	$(cc) -c main.c -o build/main.o
	$(cc) build/main.o build/lib.a -o main.out

main_shared: shared_lib
	$(cc) main.c -L. -o main.out -ldiff -Wl,-rpath=.

main_dynamic: shared_lib
	$(cc) main.c -o main.out -ldl -D dynamic

main_static_optimized: main.c static_lib
	$(cc) -c main.c -o build/main.o -O$(opt_lvl)
	$(cc) build/main.o build/lib.a -o main.out

main_shared_optimized: shared_lib
	$(cc) main.c -L. -o main.out -ldiff -Wl,-rpath=. -O$(opt_lvl)

main_dynamic_optimized: shared_lib
	$(cc) main.c -o main.out -ldl -D dynamic -O$(opt_lvl)

clean:
	rm build/* *.out *.so *.o *.a

test:
	echo > raport.txt
	echo "5 par" >> raport.txt
	./main.out create_table 5 compare_pairs a.txt:b.txt b.txt:a.txt a.txt:a.txt b.txt:b.txt a.txt:b.txt | tail -4 >> raport.txt

	echo >> raport.txt
	echo "25 par" >> raport.txt
	./main.out create_table 25 compare_pairs \
		a.txt:b.txt b.txt:a.txt a.txt:a.txt b.txt:b.txt a.txt:b.txt\
		a.txt:b.txt b.txt:a.txt a.txt:a.txt b.txt:b.txt a.txt:b.txt\
		a.txt:b.txt b.txt:a.txt a.txt:a.txt b.txt:b.txt a.txt:b.txt\
		a.txt:b.txt b.txt:a.txt a.txt:a.txt b.txt:b.txt a.txt:b.txt\
		a.txt:b.txt b.txt:a.txt a.txt:a.txt b.txt:b.txt a.txt:b.txt\
		| tail -4 >> raport.txt

	echo >> raport.txt
	echo "100 par" >> raport.txt
	./main.out create_table 100 compare_pairs \
		a.txt:b.txt b.txt:a.txt a.txt:a.txt b.txt:b.txt a.txt:b.txt\
		a.txt:b.txt b.txt:a.txt a.txt:a.txt b.txt:b.txt a.txt:b.txt\
		a.txt:b.txt b.txt:a.txt a.txt:a.txt b.txt:b.txt a.txt:b.txt\
		a.txt:b.txt b.txt:a.txt a.txt:a.txt b.txt:b.txt a.txt:b.txt\
		a.txt:b.txt b.txt:a.txt a.txt:a.txt b.txt:b.txt a.txt:b.txt\
		a.txt:b.txt b.txt:a.txt a.txt:a.txt b.txt:b.txt a.txt:b.txt\
		a.txt:b.txt b.txt:a.txt a.txt:a.txt b.txt:b.txt a.txt:b.txt\
		a.txt:b.txt b.txt:a.txt a.txt:a.txt b.txt:b.txt a.txt:b.txt\
		a.txt:b.txt b.txt:a.txt a.txt:a.txt b.txt:b.txt a.txt:b.txt\
		a.txt:b.txt b.txt:a.txt a.txt:a.txt b.txt:b.txt a.txt:b.txt\
		a.txt:b.txt b.txt:a.txt a.txt:a.txt b.txt:b.txt a.txt:b.txt\
		a.txt:b.txt b.txt:a.txt a.txt:a.txt b.txt:b.txt a.txt:b.txt\
		a.txt:b.txt b.txt:a.txt a.txt:a.txt b.txt:b.txt a.txt:b.txt\
		a.txt:b.txt b.txt:a.txt a.txt:a.txt b.txt:b.txt a.txt:b.txt\
		a.txt:b.txt b.txt:a.txt a.txt:a.txt b.txt:b.txt a.txt:b.txt\
		a.txt:b.txt b.txt:a.txt a.txt:a.txt b.txt:b.txt a.txt:b.txt\
		a.txt:b.txt b.txt:a.txt a.txt:a.txt b.txt:b.txt a.txt:b.txt\
		a.txt:b.txt b.txt:a.txt a.txt:a.txt b.txt:b.txt a.txt:b.txt\
		a.txt:b.txt b.txt:a.txt a.txt:a.txt b.txt:b.txt a.txt:b.txt\
		a.txt:b.txt b.txt:a.txt a.txt:a.txt b.txt:b.txt a.txt:b.txt\
		| tail -4 >> raport.txt

	echo >> raport.txt
	echo "500 par" >> raport.txt
	./main.out create_table 500 compare_pairs \
		a.txt:b.txt b.txt:a.txt a.txt:a.txt b.txt:b.txt a.txt:b.txt a.txt:b.txt b.txt:a.txt a.txt:a.txt b.txt:b.txt a.txt:b.txt\
		a.txt:b.txt b.txt:a.txt a.txt:a.txt b.txt:b.txt a.txt:b.txt a.txt:b.txt b.txt:a.txt a.txt:a.txt b.txt:b.txt a.txt:b.txt\
		a.txt:b.txt b.txt:a.txt a.txt:a.txt b.txt:b.txt a.txt:b.txt a.txt:b.txt b.txt:a.txt a.txt:a.txt b.txt:b.txt a.txt:b.txt\
		a.txt:b.txt b.txt:a.txt a.txt:a.txt b.txt:b.txt a.txt:b.txt a.txt:b.txt b.txt:a.txt a.txt:a.txt b.txt:b.txt a.txt:b.txt\
		a.txt:b.txt b.txt:a.txt a.txt:a.txt b.txt:b.txt a.txt:b.txt a.txt:b.txt b.txt:a.txt a.txt:a.txt b.txt:b.txt a.txt:b.txt\
		a.txt:b.txt b.txt:a.txt a.txt:a.txt b.txt:b.txt a.txt:b.txt a.txt:b.txt b.txt:a.txt a.txt:a.txt b.txt:b.txt a.txt:b.txt\
		a.txt:b.txt b.txt:a.txt a.txt:a.txt b.txt:b.txt a.txt:b.txt a.txt:b.txt b.txt:a.txt a.txt:a.txt b.txt:b.txt a.txt:b.txt\
		a.txt:b.txt b.txt:a.txt a.txt:a.txt b.txt:b.txt a.txt:b.txt a.txt:b.txt b.txt:a.txt a.txt:a.txt b.txt:b.txt a.txt:b.txt\
		a.txt:b.txt b.txt:a.txt a.txt:a.txt b.txt:b.txt a.txt:b.txt a.txt:b.txt b.txt:a.txt a.txt:a.txt b.txt:b.txt a.txt:b.txt\
		a.txt:b.txt b.txt:a.txt a.txt:a.txt b.txt:b.txt a.txt:b.txt a.txt:b.txt b.txt:a.txt a.txt:a.txt b.txt:b.txt a.txt:b.txt\
		a.txt:b.txt b.txt:a.txt a.txt:a.txt b.txt:b.txt a.txt:b.txt a.txt:b.txt b.txt:a.txt a.txt:a.txt b.txt:b.txt a.txt:b.txt\
		a.txt:b.txt b.txt:a.txt a.txt:a.txt b.txt:b.txt a.txt:b.txt a.txt:b.txt b.txt:a.txt a.txt:a.txt b.txt:b.txt a.txt:b.txt\
		a.txt:b.txt b.txt:a.txt a.txt:a.txt b.txt:b.txt a.txt:b.txt a.txt:b.txt b.txt:a.txt a.txt:a.txt b.txt:b.txt a.txt:b.txt\
		a.txt:b.txt b.txt:a.txt a.txt:a.txt b.txt:b.txt a.txt:b.txt a.txt:b.txt b.txt:a.txt a.txt:a.txt b.txt:b.txt a.txt:b.txt\
		a.txt:b.txt b.txt:a.txt a.txt:a.txt b.txt:b.txt a.txt:b.txt a.txt:b.txt b.txt:a.txt a.txt:a.txt b.txt:b.txt a.txt:b.txt\
		a.txt:b.txt b.txt:a.txt a.txt:a.txt b.txt:b.txt a.txt:b.txt a.txt:b.txt b.txt:a.txt a.txt:a.txt b.txt:b.txt a.txt:b.txt\
		a.txt:b.txt b.txt:a.txt a.txt:a.txt b.txt:b.txt a.txt:b.txt a.txt:b.txt b.txt:a.txt a.txt:a.txt b.txt:b.txt a.txt:b.txt\
		a.txt:b.txt b.txt:a.txt a.txt:a.txt b.txt:b.txt a.txt:b.txt a.txt:b.txt b.txt:a.txt a.txt:a.txt b.txt:b.txt a.txt:b.txt\
		a.txt:b.txt b.txt:a.txt a.txt:a.txt b.txt:b.txt a.txt:b.txt a.txt:b.txt b.txt:a.txt a.txt:a.txt b.txt:b.txt a.txt:b.txt\
		a.txt:b.txt b.txt:a.txt a.txt:a.txt b.txt:b.txt a.txt:b.txt a.txt:b.txt b.txt:a.txt a.txt:a.txt b.txt:b.txt a.txt:b.txt\
		a.txt:b.txt b.txt:a.txt a.txt:a.txt b.txt:b.txt a.txt:b.txt a.txt:b.txt b.txt:a.txt a.txt:a.txt b.txt:b.txt a.txt:b.txt\
		a.txt:b.txt b.txt:a.txt a.txt:a.txt b.txt:b.txt a.txt:b.txt a.txt:b.txt b.txt:a.txt a.txt:a.txt b.txt:b.txt a.txt:b.txt\
		a.txt:b.txt b.txt:a.txt a.txt:a.txt b.txt:b.txt a.txt:b.txt a.txt:b.txt b.txt:a.txt a.txt:a.txt b.txt:b.txt a.txt:b.txt\
		a.txt:b.txt b.txt:a.txt a.txt:a.txt b.txt:b.txt a.txt:b.txt a.txt:b.txt b.txt:a.txt a.txt:a.txt b.txt:b.txt a.txt:b.txt\
		a.txt:b.txt b.txt:a.txt a.txt:a.txt b.txt:b.txt a.txt:b.txt a.txt:b.txt b.txt:a.txt a.txt:a.txt b.txt:b.txt a.txt:b.txt\
		a.txt:b.txt b.txt:a.txt a.txt:a.txt b.txt:b.txt a.txt:b.txt a.txt:b.txt b.txt:a.txt a.txt:a.txt b.txt:b.txt a.txt:b.txt\
		a.txt:b.txt b.txt:a.txt a.txt:a.txt b.txt:b.txt a.txt:b.txt a.txt:b.txt b.txt:a.txt a.txt:a.txt b.txt:b.txt a.txt:b.txt\
		a.txt:b.txt b.txt:a.txt a.txt:a.txt b.txt:b.txt a.txt:b.txt a.txt:b.txt b.txt:a.txt a.txt:a.txt b.txt:b.txt a.txt:b.txt\
		a.txt:b.txt b.txt:a.txt a.txt:a.txt b.txt:b.txt a.txt:b.txt a.txt:b.txt b.txt:a.txt a.txt:a.txt b.txt:b.txt a.txt:b.txt\
		a.txt:b.txt b.txt:a.txt a.txt:a.txt b.txt:b.txt a.txt:b.txt a.txt:b.txt b.txt:a.txt a.txt:a.txt b.txt:b.txt a.txt:b.txt\
		a.txt:b.txt b.txt:a.txt a.txt:a.txt b.txt:b.txt a.txt:b.txt a.txt:b.txt b.txt:a.txt a.txt:a.txt b.txt:b.txt a.txt:b.txt\
		a.txt:b.txt b.txt:a.txt a.txt:a.txt b.txt:b.txt a.txt:b.txt a.txt:b.txt b.txt:a.txt a.txt:a.txt b.txt:b.txt a.txt:b.txt\
		a.txt:b.txt b.txt:a.txt a.txt:a.txt b.txt:b.txt a.txt:b.txt a.txt:b.txt b.txt:a.txt a.txt:a.txt b.txt:b.txt a.txt:b.txt\
		a.txt:b.txt b.txt:a.txt a.txt:a.txt b.txt:b.txt a.txt:b.txt a.txt:b.txt b.txt:a.txt a.txt:a.txt b.txt:b.txt a.txt:b.txt\
		a.txt:b.txt b.txt:a.txt a.txt:a.txt b.txt:b.txt a.txt:b.txt a.txt:b.txt b.txt:a.txt a.txt:a.txt b.txt:b.txt a.txt:b.txt\
		a.txt:b.txt b.txt:a.txt a.txt:a.txt b.txt:b.txt a.txt:b.txt a.txt:b.txt b.txt:a.txt a.txt:a.txt b.txt:b.txt a.txt:b.txt\
		a.txt:b.txt b.txt:a.txt a.txt:a.txt b.txt:b.txt a.txt:b.txt a.txt:b.txt b.txt:a.txt a.txt:a.txt b.txt:b.txt a.txt:b.txt\
		a.txt:b.txt b.txt:a.txt a.txt:a.txt b.txt:b.txt a.txt:b.txt a.txt:b.txt b.txt:a.txt a.txt:a.txt b.txt:b.txt a.txt:b.txt\
		a.txt:b.txt b.txt:a.txt a.txt:a.txt b.txt:b.txt a.txt:b.txt a.txt:b.txt b.txt:a.txt a.txt:a.txt b.txt:b.txt a.txt:b.txt\
		a.txt:b.txt b.txt:a.txt a.txt:a.txt b.txt:b.txt a.txt:b.txt a.txt:b.txt b.txt:a.txt a.txt:a.txt b.txt:b.txt a.txt:b.txt\
		a.txt:b.txt b.txt:a.txt a.txt:a.txt b.txt:b.txt a.txt:b.txt a.txt:b.txt b.txt:a.txt a.txt:a.txt b.txt:b.txt a.txt:b.txt\
		a.txt:b.txt b.txt:a.txt a.txt:a.txt b.txt:b.txt a.txt:b.txt a.txt:b.txt b.txt:a.txt a.txt:a.txt b.txt:b.txt a.txt:b.txt\
		a.txt:b.txt b.txt:a.txt a.txt:a.txt b.txt:b.txt a.txt:b.txt a.txt:b.txt b.txt:a.txt a.txt:a.txt b.txt:b.txt a.txt:b.txt\
		a.txt:b.txt b.txt:a.txt a.txt:a.txt b.txt:b.txt a.txt:b.txt a.txt:b.txt b.txt:a.txt a.txt:a.txt b.txt:b.txt a.txt:b.txt\
		a.txt:b.txt b.txt:a.txt a.txt:a.txt b.txt:b.txt a.txt:b.txt a.txt:b.txt b.txt:a.txt a.txt:a.txt b.txt:b.txt a.txt:b.txt\
		a.txt:b.txt b.txt:a.txt a.txt:a.txt b.txt:b.txt a.txt:b.txt a.txt:b.txt b.txt:a.txt a.txt:a.txt b.txt:b.txt a.txt:b.txt\
		a.txt:b.txt b.txt:a.txt a.txt:a.txt b.txt:b.txt a.txt:b.txt a.txt:b.txt b.txt:a.txt a.txt:a.txt b.txt:b.txt a.txt:b.txt\
		a.txt:b.txt b.txt:a.txt a.txt:a.txt b.txt:b.txt a.txt:b.txt a.txt:b.txt b.txt:a.txt a.txt:a.txt b.txt:b.txt a.txt:b.txt\
		a.txt:b.txt b.txt:a.txt a.txt:a.txt b.txt:b.txt a.txt:b.txt a.txt:b.txt b.txt:a.txt a.txt:a.txt b.txt:b.txt a.txt:b.txt\
		a.txt:b.txt b.txt:a.txt a.txt:a.txt b.txt:b.txt a.txt:b.txt a.txt:b.txt b.txt:a.txt a.txt:a.txt b.txt:b.txt a.txt:b.txt\
		| tail -4 >> raport.txt

	echo >> raport.txt
	echo "identical" >> raport.txt
	./main.out create_table 10 compare_pairs a.txt:a.txt a.txt:a.txt a.txt:a.txt a.txt:a.txt a.txt:a.txt a.txt:a.txt a.txt:a.txt a.txt:a.txt a.txt:a.txt a.txt:a.txt | tail -4 >> raport.txt

	echo >> raport.txt
	echo "similar" >> raport.txt
	./main.out create_table 10 compare_pairs a.txt:b.txt a.txt:b.txt a.txt:b.txt a.txt:b.txt a.txt:b.txt a.txt:b.txt a.txt:b.txt a.txt:b.txt a.txt:b.txt a.txt:b.txt | tail -4 >> raport.txt

	echo >> raport.txt
	echo "different" >> raport.txt
	./main.out create_table 10 compare_pairs lib.c:main.c lib.c:main.c lib.c:main.c lib.c:main.c lib.c:main.c lib.c:main.c lib.c:main.c lib.c:main.c lib.c:main.c lib.c:main.c | tail -4 >> raport.txt

	echo >> raport.txt
	echo "remove_block "
	./main.out create_table 1 compare_pairs a.txt:c.txt remove_block 0 | tail -4 >> raport.txt