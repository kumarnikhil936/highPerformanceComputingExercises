default: compile run

compile:
	mpiicc -g -Wall -o lab2 lab2.c -DMSG_LEN=33554430

run:
	mpiexec -n 2 ./lab2
