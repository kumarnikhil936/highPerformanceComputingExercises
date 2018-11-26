default: compile run

compile:
	mpiicc -g -Wall -o mand mandelbrot.c

run:
	mpiexec -n 2 ./mand 0 0 10 100
