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
	@rm -f "$(fifo)"
	@diff test.txt <(echo $$(cat conf.txt | sed -E 's/^#[0-9]+#//g' | tr -d '\n')) > /dev/null && echo 'test passed' || echo 'test failed'

testmany: SHELL:=/bin/bash
testmany: generate
	@rm -f "$(fifo)"
	mkfifo "$(fifo)"
	for i in {1..10}; do\
		sleep $$[ ( $$RANDOM % 10 )  + 1 ]s && ./producer.out "$(fifo)" "$(file)" $(buffsize) & \
	done
	./customer.out "$(fifo)" "conf.txt" $(buffsize)
	@rm -f "$(fifo)"

generate: all
	# generate random alphanumeric string
	cat /dev/urandom | tr -dc 'a-zA-Z0-9' | fold -w $(filesize) | head -n 1 | tr -d '\n' > "$(file)"

clean:
	rm -f *.out *.fifo *.txt
