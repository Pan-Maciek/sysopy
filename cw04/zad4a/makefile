cc = gcc -Wall -ggdb

all: sender catcher

sender: sender.c
	$(cc) sender.c -o sender.out

catcher: catcher.c
	$(cc) catcher.c -o catcher.out

n = 10000
test: all
	@echo "\n          [kill]"
	@./catcher.out kill - &
	@./sender.out `pidof catcher.out` $n kill
	@echo "\n          [sigqueue]"
	@./catcher.out sigqueue - &
	@./sender.out `pidof catcher.out` $n sigqueue
	@echo "\n          [sigrt]"
	@./catcher.out sigrt - &
	@./sender.out `pidof catcher.out` $n sigrt


clean:
	rm -Rf *.out
