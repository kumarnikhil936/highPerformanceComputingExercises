default: compile run

compile:
	mpiicc -g -Wall -o mand mandelbrot.c

run:
	mpiexec -n 8 ./mand -1.5 -0.5 0.65 10000
