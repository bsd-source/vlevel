CC=g++
CFLAGS=-Wall -O3

vlevel: main.o volumeleveler.o commandline.o
	$(CC) $(CFLAGS) -o vlevel main.o volumeleveler.o commandline.o

volumeleveler.o: volumeleveler.cpp volumeleveler.h
	$(CC) $(CFLAGS) -c volumeleveler.cpp

main.o: main.cpp volumeleveler.h commandline.h
	$(CC) $(CFLAGS) -c main.cpp

commandline.o: commandline.cpp commandline.h
	$(CC) $(CFLAGS) -c commandline.cpp

clean:
	rm -f *.o vlevel
