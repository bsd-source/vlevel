CC=g++
CFLAGS=-Wall -O3

all: vlevel-bin

vlevel-bin: vlevel-bin.o volumeleveler.o commandline.o
	$(CC) $(CFLAGS) -o vlevel-bin vlevel-bin.o volumeleveler.o commandline.o

volumeleveler.o: volumeleveler.cpp volumeleveler.h
	$(CC) $(CFLAGS) -c volumeleveler.cpp

vlevel-bin.o: vlevel-bin.cpp volumeleveler.h commandline.h
	$(CC) $(CFLAGS) -c vlevel-bin.cpp

commandline.o: commandline.cpp commandline.h
	$(CC) $(CFLAGS) -c commandline.cpp

clean:
	rm -f *.o vlevel
