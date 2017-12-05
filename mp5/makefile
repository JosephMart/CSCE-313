CC = g++
CXXFLAGS = -std=c++11 -Wall -Wextra -pedantic

all: dataserver client

mutex.o: mutex.H mutex.cc
	$(CC) $(CXXFLAGS) -c -g mutex.cc -lpthread

semaphore.o: semaphore.cc semaphore.H
	$(CC) $(CXXFLAGS) -c -g semaphore.cc -lpthread

reqchannel.o: reqchannel.H reqchannel.cc
	$(CC) $(CXXFLAGS) -c -g reqchannel.cc

netreqchannel.o: netreqchannel.H netreqchannel.cc
	$(CC) $(CXXFLAGS) -c -g netreqchannel.cc

dataserver: dataserver_323009961.cc reqchannel.o netreqchannel.o
	$(CC) $(CXXFLAGS) -g -o dataserver dataserver_323009961.cc reqchannel.o netreqchannel.o -lpthread

client: client_323009961.cc reqchannel.o BoundedBuffer.h mutex.o semaphore.o netreqchannel.o
	$(CC) $(CXXFLAGS) -g -o client client_323009961.cc reqchannel.o BoundedBuffer.h mutex.o semaphore.o netreqchannel.o -lpthread

BoundedBufferUnitTests: BoundedBufferUnitTests.cc BoundedBuffer.h mutex.o semaphore.o
	$(CC) $(CXXFLAGS) -g -o BoundedBufferUnitTests BoundedBufferUnitTests.cc BoundedBuffer.h mutex.o semaphore.o -lpthread

clean:
	rm -rf client dataserver BoundedBufferUnitTests *.o fifo*
