CC = g++
CXXFLAGS = -std=c++11

all: dataserver client

mutex.o: mutex.H mutex.cc
	$(CC) $(CXXFLAGS) -c -g mutex.cc -lpthread

semaphore.o: semaphore.cc semaphore.H
	$(CC) $(CXXFLAGS) -c -g semaphore.cc -lpthread

reqchannel.o: reqchannel.H reqchannel.cc
	$(CC) $(CXXFLAGS) -c -g reqchannel.cc

dataserver: dataserver_323009961.cc reqchannel.o
	$(CC) $(CXXFLAGS) -g -o dataserver dataserver_323009961.cc reqchannel.o -lpthread

client: client_323009961.cc reqchannel.o BoundedBuffer.h mutex.o semaphore.o
	$(CC) $(CXXFLAGS) -g -o client client_323009961.cc reqchannel.o BoundedBuffer.h mutex.o semaphore.o -lpthread

BoundedBufferUnitTests: BoundedBufferUnitTests.cc BoundedBuffer.h mutex.o semaphore.o
	$(CC) $(CXXFLAGS) -g -o BoundedBufferUnitTests BoundedBufferUnitTests.cc BoundedBuffer.h mutex.o semaphore.o -lpthread

clean:
	rm -rf client dataserver BoundedBufferUnitTests *.o fifo*
