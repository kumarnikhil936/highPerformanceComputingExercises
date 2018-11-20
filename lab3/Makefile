default: compile_pi run_pi compile_bf run_bf

compile_pi:
	mpiicc -g -Wall -o pi pi.c

run_pi:
	mpiexec -n 8 ./pi

compile_bf:
	mpiicc -g -Wall -o bf butterfly.c

run_bf:
	mpiexec -n 8 ./bf
