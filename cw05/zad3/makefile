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
test: generate
	@rm -f "$(fifo)"
	mkfifo "$(fifo)"
	./producer.out "$(fifo)" "$(file)" $(buffsize) &
	./customer.out "$(fifo)" "conf.txt" $(buffsize)
	@diff -w test.txt <(echo $$(sed -E 's/^#[0-9]+#//g' conf.txt)) > /dev/null && echo 'test passed' || echo 'test failed'

testmany: generate
	@rm -f "$(fifo)"
	$(cc) tester.c -o tester.out
	./tester.out

generate: all
	# generate random alphanumeric string
	cat /dev/urandom | tr -dc 'a-zA-Z0-9' | fold -w $(filesize) | head -n 1 | tr -d '\n' > "$(file)"

clean:
	rm -f *.out *.fifo *.txt
