vlevel: main.o volumeleveler.o
	g++ -Wall -o vlevel main.o volumeleveler.o

volumeleveler.o: volumeleveler.cpp volumeleveler.h
	g++ -Wall -c volumeleveler.cpp

main.o: main.cpp volumeleveler.h
	g++ -Wall -c main.cpp

clean:
	rm -f *.o vlevel
