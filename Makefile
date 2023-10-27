simulation: simulation.cpp scheduler.cpp DES.cpp
	 g++ -g simulation.cpp DES.cpp scheduler.cpp randomizer.cpp -o simulation # I always compile with -g to enable debugging
clean:
	 rm -f simulation *~
