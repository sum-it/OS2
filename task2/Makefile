CC=gcc
CFLAGS= --std=c99 -Wall -Werror -O1
SOURCE= 2_1.c
OUTFILE=prog
SHFILE=test_script.sh
count=5
LOG_FILE=run.log

all: exec

.PHONY: all

prog: $(SOURCE)
	$(CC) $(CFLAGS) -o $(OUTFILE) $(SOURCE)
exec: prog
	./$(OUTFILE)

#executes prog traget 5 times
test:prog
#use shell script to execute the target
	./$(SHFILE) -n 5 | tee -a run.log

.PHONY: clean
clean:
	rm -f $(OUTFILE) $(LOG_FILE)
