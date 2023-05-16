if [[ -z "${RELEASE}" ]];
then
	gcc -g *.c -o chess
else
	gcc *.c -o chess
fi

