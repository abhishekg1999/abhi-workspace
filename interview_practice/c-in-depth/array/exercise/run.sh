#bin/bash
if [ $1 == "help" ]
then
	echo "pass argument ./run.sh .c file"
else
	cc $1
	echo "$1 running..."
	./a.out

fi
