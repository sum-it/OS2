CC=gcc
CFLAGS= --std=c99 -Wall -Werror -O1
SOURCE= 2_3.c
OUTFILE=prog
SHFILE=test_script.sh
COUNT=5
LOG_FILE=run.log
DIR=x

all: exec

.PHONY: all

prog: $(SOURCE)
	$(CC) $(CFLAGS) -o $(OUTFILE) $(SOURCE)
exec:prog
	./$(OUTFILE) -n $(COUNT) -d $(DIR)| tee -a $(LOG_FILE) 

#executes prog traget 5 times
test:prog
#use shell script to execute the target
	./$(SHFILE) -n $(COUNT) -d $(DIR) 

.PHONY: clean
clean:
	rm -f $(OUTFILE) $(LOG_FILE)
