cc = gcc -Wall -ggdb
fifo = test.fifo
file = test.txt
filesize = 25
buffsize = 5

all: customer producer

customer:
	$(cc) customer.c -o customer.out

producer:
	$(cc) producer.c -o producer.out

test: SHELL:=/bin/bash
test: all
	@rm -f "$(fifo)"
	mkfifo "$(fifo)"
	# generate random alphanumeric string
	cat /dev/urandom | tr -dc 'a-zA-Z0-9' | fold -w $(filesize) | head -n 1 > "$(file)"
	./producer.out "$(fifo)" "$(file)" $(buffsize)&
	./customer.out "$(fifo)" "conf.txt" $(buffsize)
	@rm -f "$(fifo)"
	@diff test.txt <( echo $$(cat conf.txt | sed -E 's/^#[0-9]+#//g' | tr -d '\n')) > /dev/null && echo 'test passed' || echo 'test failed'

clean:
	rm -f *.out *.fifo *.txt
