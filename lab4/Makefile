default: compile run

compile:
	mpiicc -g -Wall -o mand mandelbrot.c

run:
	mpiexec -n 2 ./mand -1.5 -0.5 0.65 10000

run_default: 
	mpiexec -n 2 ./mand 
