CFLAGS = -Wall
CC = gcc

main: main.o

clean:
	rm -f main.o main

test:  main
	@echo -e "\nTESTING\n"
	@python -c "import socket,random,string,time; random.seed(1234); output=''.join(random.choice(string.lowercase) for _ in range(32)); s=socket.socket(); s.bind(('127.0.0.1',7474)); s.listen(1); c,_=s.accept(); c.send(output); time.sleep(1); c.close(); s.close()" &
	@sleep 0.1
	./main
	@sleep 1

all: main test

