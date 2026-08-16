#bin/bash
if [ -z "$1" ] || [ $1 == "help" ]
then
	echo "Usage: ./run.sh <filename.c>"
else
	cc $1 
	echo "running... [$1]"
	./a.out

fi
