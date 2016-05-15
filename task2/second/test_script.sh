#!/bin/bash
EXE_F=prog
COUNT=1
run=5
counter=1
succ=0
fail=0
LOG_FILE=run.log
DIR=x
#set -x
if [ -f $LOG_FILE ]
then 
	rm $LOG_FILE
fi
while [[ $# > 1 ]]
do
key="$1"

case $key in
    -n)
    COUNT="$2"
    shift # past argument
    ;;
    -d)
    DIR="$2"
    shift # past argument
    ;;
    *)
            # unknown option
    ;;
esac
shift # past argument or value
done

while [ "$counter" -le $run ] 
do
	./$EXE_F -n $COUNT -d $DIR > $LOG_FILE
done
#set +x
