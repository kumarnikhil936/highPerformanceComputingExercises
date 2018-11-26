default: compile run

compile:
	mpiicc -g -Wall -o mand mandelbrot.c

run:
	mpiexec -n 16 ./mand -1.0 -1.5 0.55 100000
