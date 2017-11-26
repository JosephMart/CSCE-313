# makefile

all:
	gcc -lm -O0 -o memtest *.c

utils.o: utils.c
	gcc -c -g -lm utils.c

free_list.o: free_list.c free_list.h utils.o
	gcc -c -g free_list.c utils.o

my_allocator.o: my_allocator.c my_allocator.h free_list.o utils.o
	gcc -c -g my_allocator.c free_list.o utils.o

memtest: memtest.c my_allocator.o free_list.o
	gcc -o memtest memtest.c my_allocator.o free_list.o

clean:
	rm *.o memtest