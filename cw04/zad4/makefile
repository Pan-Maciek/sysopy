cc = gcc -Wall -ggdb

all: sender catcher

sender: sender.c
	$(cc) sender.c -o sender.out

catcher: catcher.c
	$(cc) catcher.c -o catcher.out

test: all
	@echo "kill:"
	@./catcher.out kill&
	@./sender.out `pidof catcher.out` 1000 kill
	@echo "sigqueue:"
	@./catcher.out sigqueue&
	@./sender.out `pidof catcher.out` 1000 sigqueue
	@echo "sigrt:"
	@./catcher.out sigrt&
	@./sender.out `pidof catcher.out` 1000 sigrt


clean:
	rm -Rf *.out
