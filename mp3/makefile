# makefile
CXXFLAGS = -Wall -Wextra -pedantic

all: dataserver client

clean:
	rm client simpleclient dataserver *.o

reqchannel.o: reqchannel.H reqchannel.cc
	g++ $(CXXFLAGS) -c -g reqchannel.cc

dataserver: dataserver.cc reqchannel.o 
	g++ $(CXXFLAGS) -g -o dataserver dataserver.cc reqchannel.o -lpthread

simpleclient: simpleclient.cc reqchannel.o
	g++ $(CXXFLAGS) -g -o simpleclient simpleclient.cc reqchannel.o

client: client.cc reqchannel.o
	g++ $(CXXFLAGS) -g -o client client.cc reqchannel.o -lpthread
