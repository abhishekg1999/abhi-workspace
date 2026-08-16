#bin/bash
if [ -z "$1" ] || [ $1 == "help" ]
then
	echo "Usage: ./run.sh <filename.c>"
else
	echo "Program... [$1]"
	cc $1 && ./a.out

fi
