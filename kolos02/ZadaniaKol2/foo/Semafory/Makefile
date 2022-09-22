CFLAGS = -Wall
CC = gcc

main: semafor.o

clean:
	rm -f ./semafor.o ./semafor

test:  semafor
	@echo -e "\n\nTESTING\n\n"
	./semafor good
	@echo -e "=========================================================="
	./semafor bad

all: semafor test
