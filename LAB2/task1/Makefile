CC=gcc
CFLAGS= --std=c99 -O1 -Wall -Wextra  
LIBS= -lcriu
SOURCE= 1.c
OUTFILE= test
LOG_FILE=run.log

all: exec

.PHONY: all

test : $(SOURCE)
	$(CC) $(CFLAGS) -o $(OUTFILE) $(SOURCE) $(LIBS)
exec:test
	sudo criu service -d
	sudo ./$(OUTFILE) 

#executes prog traget 5 times
#use shell script to execute the target

.PHONY: clean
clean:
	rm -f $(OUTFILE) $(LOG_FILE)
