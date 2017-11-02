CC = g++
CXXFLAGS = -Wall -Wextra -pedantic

all: dataserver client

mutex.o: mutex.H mutex.cc
	$(CC) $(CXXFLAGS) -c -g mutex.cc -lpthread

mutex_guard.o: mutex_guard.H mutex_guard.cc
	$(CC) $(CXXFLAGS) -c -g mutex_guard.cc

semaphore.o: semaphore.cc semaphore.H
	$(CC) $(CXXFLAGS) -c -g semaphore.cc -lpthread

BoundedBuffer.o: BoundedBuffer.h BoundedBuffer.cc
	$(CC) $(CXXFLAGS) -c -g BoundedBuffer.cc

reqchannel.o: reqchannel.H reqchannel.cc
	$(CC) $(CXXFLAGS) -c -g reqchannel.cc

dataserver: dataserver.cc reqchannel.o
	$(CC) $(CXXFLAGS) -g -o dataserver dataserver.cc reqchannel.o -lpthread

# simpleclient: simpleclient.cc reqchannel.o
#	 g++ -g -o simpleclient simpleclient.cc reqchannel.o

client: client.cc reqchannel.o BoundedBuffer.o
	$(CC) $(CXXFLAGS) -g -o client client.cc reqchannel.o BoundedBuffer.o -lpthread

BoundedBufferUnitTests: BoundedBufferUnitTests.cc BoundedBuffer.o mutex.o semaphore.o
	$(CC) $(CXXFLAGS) -g -o BoundedBufferUnitTests BoundedBufferUnitTests.cc BoundedBuffer.o mutex.o semaphore.o -lpthread

clean:
	rm -rf client dataserver BoundedBufferUnitTests *.o

bound_buffer_tests: BoundedBufferUnitTests.cc BoundedBuffer.o
	g++ $(CXXFLAGS) -g -o bbtests BoundedBuffer.cc BoundedBuffer.o -lpthread