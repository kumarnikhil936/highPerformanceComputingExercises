default: serial

parallel:
	g++ -fopenmp g_omp.cpp -o g_omp
	./g_omp

serial:
	gcc serial.cpp -o serial
	./serial

clean:
	rm g_omp g_serial
