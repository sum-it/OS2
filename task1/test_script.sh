#!/bin/bash
EXE_F=prog
count=5
counter=1
#set -x
while [ "$counter" -le $count ] 
do
	./$EXE_F |tee -a run.log
	counter=`expr $counter + 1 `
done
#set +x
