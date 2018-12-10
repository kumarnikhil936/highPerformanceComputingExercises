default: parallel

parallel:
	gcc-7 -fopenmp -o gol gameoflife.c
	./gol input64.ppm 10 

clean:
	rm gol output*ppm
