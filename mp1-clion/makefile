# makefile

all: memtest

my_allocator.o : my_allocator.c my_allocator.h
	g++ -c -g my_allocator.c

memtest: memtest.c my_allocator.o
	g++ -o memtest memtest.c my_allocator.o

