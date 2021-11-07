
# The main all target
all: main.o mutex.o utils.o dependency_struct.o
		g++ -o deadlock_prediction main.o mutex_detect.o utils.o dependency_struct.o
		./deadlock_prediction $(N)


main.o:	main.cpp mutex_detect.h utils.h dependency_struct.h
		g++ -c main.cpp

mutex.o:	mutex_detect.cpp mutex_detect.h utils.h
		g++ -c mutex_detect.cpp
		
		
dependency_struct.o:	dependency_struct.cpp dependency_struct.h utils.h
		g++ -c dependency_struct.cpp

		
utils.o:	utils.h utils.cpp
		g++ -c utils.cpp

