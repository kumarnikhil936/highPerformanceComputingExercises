default: compile

compile:
	gcc-7 -Wall -fopenmp -o Rushhour Car.cpp State.cpp StateManager.cpp RushHour.cpp

run:
	./Rushhour
clean:
	rm Rushhour
