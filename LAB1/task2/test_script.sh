#!/bin/bash
EXE_F=prog
count=5
counter=1
succ=0
fail=0
LOG_FILE=run.log
#set -x
if [ -f $LOG_FILE ]
then 
	rm $LOG_FILE
fi

#2>&1
#1>$LOG_FILE
exec &>$LOG_FILE

while [ "$counter" -le $count ] 
do
	./$EXE_F 2 #|tee -a $LOG_FILE
	if [ $? -eq 0 ]; then 
		 succ=`expr $succ + 1`
	else 
		fail=`expr $fail + 1`
	fi
	counter=`expr $counter + 1 `
done
echo "Succeeded $succ times and Failed $fail times"
#set +x
