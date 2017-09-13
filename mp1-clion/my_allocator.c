/*
    File: my_allocator.c

    Author: Joseph Martinsen
            Department of Computer Science
            Texas A&M University
    Date  : <date>

    Modified: 

    This file contains the implementation of the module "MY_ALLOCATOR".

*/

/*--------------------------------------------------------------------------*/
/* DEFINES */
/*--------------------------------------------------------------------------*/

/* -- (none) -- */

/*--------------------------------------------------------------------------*/
/* INCLUDES */
/*--------------------------------------------------------------------------*/

#include<stdlib.h>
#include "my_allocator.h"
#include "free_list.h"

// TODO: remove below
#include<stdio.h>


/*--------------------------------------------------------------------------*/
/* DATA STRUCTURES */
/*--------------------------------------------------------------------------*/

/* -- (none) -- */

/*--------------------------------------------------------------------------*/
/* CONSTANTS */
/*--------------------------------------------------------------------------*/
// Points to head of the linked list
static FL_HEADER* FREE_LIST_HEAD;

// Stored constant of the header size
static const int FL_HEADER_SIZE = sizeof(FREE_LIST_HEAD);

/*--------------------------------------------------------------------------*/
/* FORWARDS */
/*--------------------------------------------------------------------------*/

/* -- (none) -- */

/*--------------------------------------------------------------------------*/
/* FUNCTIONS FOR MODULE MY_ALLOCATOR */
/*--------------------------------------------------------------------------*/

unsigned int init_allocator(unsigned int _basic_block_size,
                            unsigned int _length) {
    // Initialize memory using malloc
    Addr start = malloc(_length);

    if (start && _length >= _basic_block_size ) {
        // Initialize the free list
        FREE_LIST_HEAD = (FL_HEADER *) start;
        FREE_LIST_HEAD->length = _basic_block_size;
        FREE_LIST_HEAD->head = NULL;
        FREE_LIST_HEAD->tail = NULL;

        // Split the heap into _basic_block_sizes
        FL_HEADER *prev_fl = FREE_LIST_HEAD;
        FL_HEADER *temp_fl;
        unsigned int remaining_length = _length - _basic_block_size;
        unsigned int block_to_allocate = 0;

        while (remaining_length) {
            // Determine the upcoming block size
            block_to_allocate = _basic_block_size;
            if (_basic_block_size + FL_HEADER_SIZE > remaining_length) {
                if (FL_HEADER_SIZE > remaining_length) {
                    break;
                }
                block_to_allocate = remaining_length;
            }

            // Add a new FL_HEADER to the free list
            temp_fl = (FL_HEADER*)((char*) prev_fl + prev_fl->length + FL_HEADER_SIZE);
            temp_fl->length = block_to_allocate;
            temp_fl->head = NULL;
            temp_fl->tail = prev_fl;
            prev_fl->head = temp_fl;

            // Increment traversal variables
            prev_fl = temp_fl;
            remaining_length -= block_to_allocate;
        }
        return _length;
    }
    return 0;
}

Addr my_malloc(size_t _length) {
    // Find a block of appropriate size
    FL_HEADER *current = FREE_LIST_HEAD;
    while (current->length < _length) {
        if (current->head == NULL) {
            return 0;
        }
        current = current->head;
    }

    // Split the block as needed
    if (current->length - FL_HEADER_SIZE  > _length) {
        FL_HEADER *extra_block = (FL_HEADER*)((char*) current + _length + FL_HEADER_SIZE);
        extra_block->length = (unsigned int) (_length - FL_HEADER_SIZE);
        extra_block->head = current;
        extra_block->tail = current->tail;
        current->tail = extra_block;
    }
    FL_remove(current);
    return (Addr) current;
}

int my_free(Addr _a) {
    /* Same here! */
    FL_add(FREE_LIST_HEAD, _a);
    return 0;
}

