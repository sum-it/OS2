CC=gcc
CFLAGS= --std=c99 -Wall -Werror -O1
SOURCE= 1_2.c
OUTFILE=prog

all: exec

.PHONY: all

prog: $(SOURCE)
	$(CC) $(CFLAGS) -o $(OUTFILE) $(SOURCE)
exec: prog
	./$(OUTFILE)

#executes prog traget 5 times
test:prog
#use shell script to execute the target

.PHONY: clean
clean:
	rm -f $(OUTFILE)
